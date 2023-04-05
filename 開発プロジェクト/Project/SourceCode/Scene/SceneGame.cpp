#include "SceneGame.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <bitset>

#include "Arithmetic/Arithmetic.h"
#include "Arithmetic/MyMath.h"
#include "Debug/DebugGUI.h"
#include "Debug/DebugPrimitive.h"
#include "Game/General/GameSystem.h"
#include "Game/Actor/ActorManager.h"
#include "Game/HUD/HUDManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/General/PlayerController.h"
#include "Game/General/SceneFade.h"
#include "Game/General/Pause.h"
#include "Game/General/Title.h"
#include "Game/General/GameText.h"
#include "General/AtaiEngine.h" // グラフィックエンジン
#include "Graphics/Effekseer/EffekseerManager.h" 
#include "Graphics/General/DXHelperUsers.h" // 自作関数作成ヘルパ
#include "Graphics/General/Graphics.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/Model/GLTFModelResource.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/Resource/RenderState.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/Camera/OrthographicCameraEX.h"
#include "Graphics/Resource/Camera/Shake/CameraShakeDamaged.h"
#include "Graphics/PostProcess/DeferredLighting.h"
#include "Graphics/PostProcess/PostEffect.h"
#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Text/TextRenderer.h"

#include "Game/Actor/Actor.h"
#include "Game/Component/Health/Health.h"

#include "Input/Input.h"
#include "Utility/RandomFunction.h"
#include "Utility/FileDialogBox.h"

/* --- 定義 ---- */

SceneGame::SceneGame()
	: is_game_retry(false)
{
#ifdef _DEBUG
	can_control_camera = false;
#endif // _DEBUG
}

void SceneGame::Initialize()
{
	auto& graphics{ Graphics::GetInstance() };

	ID3D11Device* device{ MySystem.GetDevice() };
	auto [screen_width, screen_height] { Engine::GetInstance().GetScreenSize<UINT>() };

	// レンダーターゲットの作成
	main_render_target = std::make_shared<RenderTarget>();
	main_depth_stencil = std::make_shared<DepthStencil>();

	// HDR（32は重いので16）
	main_render_target->Create(device, screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	main_depth_stencil->Create(device, screen_width, screen_height, DXGI_FORMAT_R32_FLOAT);
	 
#ifndef _DEBUG
	// モデルデータの先行読み込み (プレイ中にカクつかないように)
	{
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Enemy/Skeleton/skeleton_skinned.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Player/Paladin/paladin6.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Enemy/Wolf/wolf3.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Enemy/Spider/spider4.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Enemy/Spider/projectile.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Item/Weapon/Sword/sword1.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Enemy/Boss/boss2.gltf");
		GLTFModelResource::GetInstance().LoadModelData(device, "Data/Model/GLTF/Floor/floor_impact.gltf");
	}

	// エフェクトの先行読み込み
	{
		std::lock_guard<std::mutex> mtx(MySystem.GetMutex());
		Graphics::GetInstance().GetEffekseerManager()->LoadEffectRecursive(L"Data/Effect");
	}
#endif

	// システムの初期化
	GameSystem::GetInstance().Initialize(device);

	// BGMの再生
	bgm.Play(L"Data/Audio/SE/ノイズ置き場/atmosphere2-converted.wav", 0.1f, true);

#ifdef _DEBUG

	// デバッグ用のカメラ
	{
		const auto [wf, hf] { MySystem.GetScreenSize<float>()};
		constexpr float near_z{ 0.001f };
		constexpr float far_z{ 1000.0f };
		constexpr bool is_fix_up{ true }; // カメラの上ベクトルの固定
		constexpr float view_angle_deg{ 45.0f }; // 視野角

		camera = std::make_shared<PerspectiveCamera>(
			Vector3(0, 2.0f, -5.0f),
			Vector3::ZeroVector(),
			near_z,
			far_z,
			is_fix_up,
			view_angle_deg,
			wf / hf);
	}

#endif // _DEBUG
}

void SceneGame::EndProcess()  
{
	// ゲームシステムの終了処理
	GameSystem::GetInstance().EndProcess();

	// モデルリソースの解放
	GLTFModelResource::GetInstance().ReleaseCache();
}


void SceneGame::Update(float elapsed_time)
{
	auto [width, height] { MySystem.GetScreenSize<float>() };

	const auto& keyboard{ Input::GetInstance().GetKeyboard() };
	auto& pgui{ DebugGUI::GetInstance().GetPerformanceGUI() };

	// スクリーンモードを変更した場合の処理
	if (PauseSystem::GetInstance().IsChangeScreen())
	{
		// Window <--> Fullscreen
		MySystem.ChangeScreenMode(!MySystem.IsFullscreen());

		// 画面サイズが変更されるため、
		ResetScreen();

		// スクリーンモード変更フラグをfalseに
		PauseSystem::GetInstance().ChangedScreen();
		return;
	}

	// ゲームオーバー時の処理
	if (GameSys.IsGameOver())
	{
		AreaChange::GetInstance().FadeIn(0.5f);

		// ほぼフェードインしきった場合タイトルへ戻る
		if (AreaChange::GetInstance().GetFade() > 0.95f)
		{
			is_game_retry = true;
		}
	}

	// タイトルに戻る処理
	if (is_game_retry)
	{
		GameRetry(MySystem.GetDevice());
		is_game_retry = false;
	}

	// タイトルの場合の処理
	if (TitleSystem::GetInstance().IsTitle())
	{
		if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
		{
			GameSys.GameStart();
			AreaChange::GetInstance().FadeOut(2.0f);
		}
	}

	// ポーズ状態の制御
	PauseSystem::GetInstance().WaitForPauseKey(GamePad::DPAD_START);

	PauseSystem::GetInstance().Update(elapsed_time, width, height);

#ifdef _DEBUG
	// デバッグ用のカメラ操作
	ControlCamera(elapsed_time);
#endif // _DEBUG

	Graphics::GetInstance().GetDeferredLighting()->Update();

	AreaChange::GetInstance().Update(elapsed_time);

	// ポーズ状態では動かない
	if (!PauseSystem::GetInstance().IsPause())
	{
		// ゲームシステムの更新処理
		GameSystem::GetInstance().Update(elapsed_time);

		// エフェクトの更新処理 ゲームシステムよりも後に処理をする。
		Graphics::GetInstance().GetEffekseerManager()->Update(elapsed_time);
	}
}

void SceneGame::Render(ID3D11DeviceContext* dc)
{
	// レンダーターゲットとシェーダーリソースの同時アクセスを無効化する
	// これがないと出力ログにエラーが出る(フレームバッファ使用時)
	{
		ID3D11RenderTargetView* null_rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
		dc->OMSetRenderTargets(_countof(null_rtvs), null_rtvs, 0);
		ID3D11ShaderResourceView* null_srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
		dc->VSSetShaderResources(0, _countof(null_rtvs), null_srvs);
		dc->PSSetShaderResources(0, _countof(null_rtvs), null_srvs);
		dc->GSSetShaderResources(0, _countof(null_rtvs), null_srvs);
	}

	auto [screen_widthf, screen_heightf] { MySystem.GetScreenSize<float>() };

	auto& graphics{ Graphics::GetInstance() };
	auto& frame_buffer{ graphics.GetFrameBuffer() };
	auto& deferred_lighting{ graphics.GetDeferredLighting() };
	auto& texture_renderer{ graphics.GetTextureRenderer() };

	// カメラの設定
	frame_buffer->SetSceneCB(dc, nullptr);

	// 影のキャスト
	graphics.GetDeferredLighting()->ShadowCast(dc);

	// ジオメトリバッファに描画
	deferred_lighting->ActivateGeometryBuffer(dc);
	{
		GameSystem::GetInstance().GetStageManager()->Render(dc);
		GameSystem::GetInstance().GetActorManager()->Render(dc);
	}

	// エフェクトの描画
	// エフェクトはジオメトリバッファのEmmisiveパラメータに書き込まれる。
	if (const auto& frame_camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera())
	{
		deferred_lighting->ActivateBufferForEffekseer(dc);
		Graphics::GetInstance().GetEffekseerManager()->Render(frame_camera->GetView(), frame_camera->GetProjection());
	}

	// ライティング
	deferred_lighting->Lighting(dc);

	// 時間反映率によって画面の彩度を変化させる
	{
		auto param{ Graphics::GetInstance().GetPostEffect()->GetColorFilter().GetFilterParam() };
		param.saturation = GameSys.GetTimeLapseRatio();
		Graphics::GetInstance().GetPostEffect()->GetColorFilter().SetFilterParam(param);
	}

	// ポストエフェクト(ブルーム、カラーフィルター)
	graphics.GetPostEffect()->Apply(dc, deferred_lighting->GetTexture());

	// メインレンダーターゲットにライティング結果を描画する
	frame_buffer->Set(
		dc, 
		screen_widthf, 
		screen_heightf,
		nullptr,//main_depth_stencil, 
		0,//D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		main_render_target);
	
	//sprite_shader.Activate(dc);
	texture_renderer->BeginDraw(dc);
	texture_renderer->Draw(dc, graphics.GetPostEffect()->GetTexture());

	
	// カメラの設定
	frame_buffer->SetSceneCB(dc, nullptr);
	DebugPrimitive::GetInstance().Render(dc);

	// スワップチェーンに描画
	MySystem.ActivateSwapChainBuffer();

	// ゲーム画面
	{
		texture_renderer->BeginDraw(dc);
		texture_renderer->Draw(dc, main_render_target->GetTexture());
		texture_renderer->EndDraw(dc);

		// テキストの描画
		GameSys.GetGameText()->Render(MySystem.GetDeviceContext2D(), GameText::Layer::GS);
	}

	// フェード
	AreaChange::GetInstance().Render(dc, screen_widthf, screen_heightf);

	// HUD, タイトル
	{
		GameSys.GetHUDManager()->Render(dc);

		// 内部でタイトルかどうかの判定を行う
		TitleSystem::GetInstance().Render(dc); 

		// テキストの描画
		GameSys.GetGameText()->Render(MySystem.GetDeviceContext2D(), GameText::Layer::HUD);
	}

	// ポーズ状態の描画処理、ポーズ状態の時のみ描画
	{
		PauseSystem::GetInstance().Render(dc, screen_widthf, screen_heightf);

		// テキストの描画
		GameSys.GetGameText()->Render(MySystem.GetDeviceContext2D(), GameText::Layer::Opt);
	}

}

void SceneGame::ImGuiRender()
{
#ifdef _DEBUG


	static bool show_geometry_buffer{ false };
	static bool is_fullscreen{ false };
	static int screen_size[2]{ 1600, 900 };
	static bool present_disallow_tearing{ false };

	static bool show_scene{ false };

	if (Input::GetInstance().GetKeyboard().GetKeyState(VK::F3) & Keyboard::Toggle)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::Checkbox(TO_CHAR(u8"フルスクリーン"), &is_fullscreen))
				{
					MySystem.ChangeScreenMode(is_fullscreen);
					ResetScreen();
				}

				if (ImGui::TreeNode(TO_CHAR(u8"ウィンドウサイズ")))
				{
					ImGui::Text(TO_CHAR(u8"ウィンドウサイズを変更するとカーソル座標がおかしくなる？"));
					ImGui::SliderInt("Width", &screen_size[0], 0, 1920);
					ImGui::SliderInt("Height", &screen_size[1], 0, 1080);

					if (ImGui::Button("Apply")) { MySystem.Resize(static_cast<UINT>(screen_size[0]), static_cast<UINT>(screen_size[1])); }
					ImGui::TreePop();
				}

				if (ImGui::Checkbox(TO_CHAR(u8"固定フレームレート"), &present_disallow_tearing))
				{
					if (present_disallow_tearing)
					{
						MySystem.SetSwapChainPresentFlag(0);
					}
					else
					{
						// 可変フレームレート
						MySystem.SetSwapChainPresentFlag(DXGI_PRESENT_ALLOW_TEARING);
					}
				}

				if (ImGui::TreeNode(TO_CHAR(u8"コントローラー")))
				{
					const auto& pad{ Input::GetInstance().GetGamePad() };

					bool connecting{ pad.IsConnectingController() };
					ImGui::Checkbox(TO_CHAR(u8"接続"), &connecting);

					std::bitset<32> input(pad.GetButton());
					ImGui::Text(input.to_string().c_str());
					auto [lx, ly] { pad.GetStickL()};
					auto [rx, ry] { pad.GetStickR()};
					ImGui::Text("LX %f, LY %f, RX %f, RY %f", lx, ly, rx, ry);

					ImGui::TreePop();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				ImGui::Checkbox(TO_CHAR(u8"表示"), &show_scene);
				ImGui::Checkbox(TO_CHAR(u8"カメラ操作"), &can_control_camera);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Graphics"))
			{
				ImGui::Checkbox("ShowGBuffer", &show_geometry_buffer);

				ImGui::Separator();

				Graphics::GetInstance().GetPostEffect()->ImGuiRender();

				ImGui::Separator();

				Graphics::GetInstance().GetEffekseerManager()->ImGuiRender();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game"))
			{

				if (ImGui::Button("Reset"))
				{
					is_game_retry = true;
				}

				GameSystem::GetInstance().ImGuiRenderOnMainMenu();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	{
		if (show_scene) { ImGuiScene(); };
		if (show_geometry_buffer) { Graphics::GetInstance().GetDeferredLighting()->ImGuiRender(); }
		if (can_control_camera) { ImGUiCameraControl(); }
	}

	// デバッグデータの表示
	DebugGUI::GetInstance().ImGuiRender();
	GameSystem::GetInstance().ImGuiRender();
#endif // _DEBUG
}

void SceneGame::GameRetry(ID3D11Device* device)
{
	// 再生中のエフェクトをすべて破棄
	Graphics::GetInstance().GetEffekseerManager()->StopAllEffects();
	// ゲーム開始時の状態に戻す
	GameSys.Retry(device);

	AreaChange::GetInstance().Initialize(device, 0.5f);
}

void SceneGame::ResetScreen()
{
	ID3D11Device* device{ MySystem.GetDevice() };
	auto [screen_width, screen_height] { MySystem.GetScreenSize<UINT>() };

	main_render_target->Create(device, screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	main_depth_stencil->Create(device, screen_width, screen_height, DXGI_FORMAT_R32_FLOAT);
	Graphics::GetInstance().GetDeferredLighting()->Initialize(device, screen_width, screen_height);
}

#ifdef _DEBUG


void SceneGame::ControlCamera(float elapsed_time)
{
	if (!can_control_camera) return;

	const auto& cursor{ Input::GetInstance().GetCursor() };
	const auto& gamepad{ Input::GetInstance().GetGamePad() };
	const auto& keyboard{ Input::GetInstance().GetKeyboard() };
	const float wheel{ Input::GetInstance().GetVerticalWheelState() };

	const auto& cp { cursor.GetCurrentPosition() };
	const auto& dp { cursor.GetPreviousPosition() };

	const Vector3& right    { camera->GetRight()};
	const Vector3& up		{ camera->GetUp() };
	const Vector3& forward  { camera->GetForward()};

	Vector3 move_val{};
	Vector3 focus{ camera->GetFocus() };
	Vector3 eye{ camera->GetEye() };

	if (keyboard.GetKeyState(VK::LBUTTON) & Keyboard::On)
	{
		if (keyboard.GetKeyState(VK::LSHIFT) & Keyboard::On)
		{
			focus += right * static_cast<float>(cp.x - dp.x) * elapsed_time;
			focus -= up * static_cast<float>(cp.y - dp.y) * elapsed_time;
			move_val += right * static_cast<float>(cp.x - dp.x) * elapsed_time;
			move_val -= up * static_cast<float>(cp.y - dp.y) * elapsed_time;
		}
		else
		{
			move_val -= right * static_cast<float>(cp.x - dp.x) * elapsed_time;
			move_val += up * static_cast<float>(cp.y - dp.y) * elapsed_time;
		}
	}

	if (wheel > FLT_EPSILON || wheel < -FLT_EPSILON)
	{
		move_val += forward * wheel * elapsed_time;
	}

	eye += move_val;

	camera->SetEye(eye);
	camera->SetFocus(focus);

	camera->Update(elapsed_time);
}

void SceneGame::ImGuiScene()
{
	ImGui::Begin("Scene");


	const auto [width, height] { MySystem.GetScreenSize<float>() };

	camera->ImGuiRender();

	if (ImGui::Button("SetToFrameCamera"))
	{
		Graphics::GetInstance().GetFrameBuffer()->PrepareCamera(camera);
	}

	if (ImGui::TreeNode("CameraShake"))
	{
		static float shake_time{ 1.0f };
		static float amplitude{ 1.0f };
		static float frequency{ 1.0f };

		ImGui::SliderFloat(TO_CHAR(u8"揺る時間"), &shake_time, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat(TO_CHAR(u8"揺れの大きさ"), &amplitude, 0.0001f, 100.0f, "%.4f");
		ImGui::SliderFloat(TO_CHAR(u8"揺れの速さ"), &frequency, 1.0f, 10000.0f, "%.2f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Set"))
		{
			camera->SetCameraShake(std::make_unique<CameraShakeDamaged>(shake_time, amplitude, frequency));
		}

		ImGui::TreePop();
	}


	ImGui::End();
}

void SceneGame::ImGUiCameraControl()
{
	ImGui::Begin(TO_CHAR(u8"カメラ操作"), 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImGui::Text(TO_CHAR(u8"左クリック＋移動　カメラ移動"));
	ImGui::Text(TO_CHAR(u8"左クリック＋左シフト＋移動　カメラ移動＋注視点移動"));
	ImGui::Text(TO_CHAR(u8"マウスホイール　注視点までの距離"));

	if(ImGui::Button(TO_CHAR(u8"操作終了"))) { can_control_camera = false; }

	ImGui::End();
}

#endif