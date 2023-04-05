#include "GameSystem.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"

#include "Debug/DebugGUI.h"
#include "Utility/Combination.h"

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorPlayer.h"
#include "Game/Actor/ActorWeapon.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Actor/ChildActorParam.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/Collider/Collider.h"
#include "Game/Component/Item/ItemDataBase.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Camera/ComponentCamera.h"
#include "Game/Component/Light/PointLight.h"
#include "Game/General/Pause.h"
#include "Game/General/Title.h"
#include "Game/General/SceneFade.h"
#include "Game/General/BattleSystem.h"
#include "Game/General/Inventory.h"
#include "Game/General/PlayerController.h"
#include "Game/General/GameText.h"
#include "Game/HUD/HUDManager.h"
#include "Game/Stage/StageManager.h"
#include "Graphics/Resource/Camera/OrthographicCameraEX.h"

/* --- 変数 ---- */

// タイトル画面でのフェード具合
constexpr float title_fade { 0.5f };

/* --- 定義 ---- */

GameSystem::GameSystem()
	: time_lapse_ratio(1.0f), is_game_over(false)
{
}

GameSystem::~GameSystem()
{
}

void GameSystem::Initialize(ID3D11Device* device)
{
	// ゲームオブジェクトであるアクタの管理クラス
	actor_manager		= std::make_unique<ActorManager>();		
	actor_manager->Initialize(device);

	// アクタ同士の衝突処理を行うクラス
	collider_manager	= std::make_unique<ColliderManager>();  

	// ゲーム内に登場するアイテム
	item_data_base		= std::make_unique<ItemDataBase>();
	item_data_base->Initialize(device);

	// プレイヤーのアクタを簡単に取得する等、プレイヤー限定で利用する機能
	player_controller	= std::make_unique<PlayerController>();
	player_controller->Initialize();

	// 所持しているアイテムの管理クラス
	inventory			= std::make_unique<Inventory>();
	inventory->Initialize();

	// ゲームで生成されるステージの管理クラス
	stage_manager		= std::make_unique<StageManager>();
	stage_manager->Initialize(device);

	// HUD
	hud_manager			= std::make_unique<HUDManager>();
	hud_manager->Initialize(device);

	// ゲーム内テキスト
	game_text			= std::make_unique<GameText>();
	game_text->Initialize();

	// ポーズ画面関連
	PauseSystem::GetInstance().Initialize(device);

	// タイトル画面関連
	TitleSystem::GetInstance().Initialize(device);

	// フェードの初期化と初期フェード量の設定
	AreaChange::GetInstance().Initialize(device, title_fade);

	// 初期配置するアクタ、初期ステージの生成
	InitializeActor(device);


#ifdef _DEBUG
	show_inventory_imgui = false;
	is_edit_mode = false;			// ステージエディタを実行中か
#endif // _DEBUG
}

void GameSystem::EndProcess()
{
	actor_manager->EndProcess();
	stage_manager->EndProcess();
}

void GameSystem::Retry(ID3D11Device* device)
{
	// 管理クラスに登録されたアクタをすべて破棄する
	actor_manager->ClearAllActor();

	stage_manager->Retry();

	hud_manager->Reset();

	inventory->Initialize();

	TitleSystem::GetInstance().SetTitleFlag(true);
	is_game_over = false;

	// 初期アクタ
	InitializeActor(device);
}

void GameSystem::Update(float elapsed_time)
{
	auto& debug{ DebugGUI::GetInstance().GetPerformanceGUI() };
	auto& area_change{ AreaChange::GetInstance() };

	if (is_transition_stage)
	{
		constexpr float fade_delta{ 2.0f }; // フェードの速度

		player_controller->GetPlayer()->GetComponent<Health>()->SetInvincibleTimer(100.0f);

		if (area_change.IsFaded())
		{
			stage_manager->ChangeStage();

			player_controller->GetPlayer()->GetComponent<Health>()->SetInvincibleTimer(0.5f);
			area_change.FadeOut(fade_delta);

			is_transition_stage = false;
		}
	}

	time_lapse_ratio = std::lerp(time_lapse_ratio, time_lapse_delta, time_lapse_delta_t * elapsed_time);

	float fixed_time{ elapsed_time * time_lapse_ratio };

	// 経過時間反映量が0以下の時は処理を行わない（完全停止）
#ifdef _DEBUG
	if (fixed_time > FLT_EPSILON && !is_edit_mode)
#else
	if(fixed_time > FLT_EPSILON)
#endif // _DEBUG
	{
		// 物理処理（衝突した際のイベント、押し出し）
		{
			auto& m5{ debug.StartMeasure("OnHitEventActors") };
			actor_manager->UpdateCollider(fixed_time);
			collider_manager->OnHitEventActors();
			m5.StopMeasure();

			// 地形タイルイベント
			auto& m7{ debug.StartMeasure("TileEvent") };
			if (stage_manager->ExecuteFloorEvent())
			{
				stage_manager->NextStage();
			}
			m7.StopMeasure();


			// 剛体処理
			auto& m2{ debug.StartMeasure("UpdateRigidBody") };
			actor_manager->UpdateRigidBody(fixed_time);
			m2.StopMeasure();
		}
		// 行列の更新
		auto& m6{ debug.StartMeasure("UpdateTransform") };
		actor_manager->UpdateTransform();
		m6.StopMeasure();
		// インベントリの処理
		inventory->Update();
		// アニメーションの更新
		auto& m4{ debug.StartMeasure("UpdateAnimation") };
		actor_manager->UpdateAnimation(fixed_time);
		actor_manager->ExecuteActorCurrentPlayAnimationEvent();
		m4.StopMeasure();
		// ビヘイビアの更新
		auto& m1{ debug.StartMeasure("UpdateBehavior") };
		actor_manager->UpdateBehavior(fixed_time);
		player_controller->Update(fixed_time);
		m1.StopMeasure();
		// コンポーネントの更新
		auto& m3{ debug.StartMeasure("UpdateActor") };
		actor_manager->UpdateActor(fixed_time);
		m3.StopMeasure();
	}

	// ダメージ処理を行う
	BattleSystem::GetInstance().ResetHealthFlags(); // 各アクタの被ダメージフラグのリセット
	BattleSystem::GetInstance().CombatProcess();	// このフレームに設定された戦闘処理を実行する

	// プレイヤーが死亡した際に実行する処理
	player_controller->DeadPlayer(); // 内部でプレイヤーの死亡判定も行う

	hud_manager->Update(elapsed_time);

	stage_manager->Update(elapsed_time);

#ifdef DEBUG
	if (is_edit_mode)
	{
		stage_manager->UpdateEditor(elapsed_time);
	}
#endif // DEBUG

}

void GameSystem::ImGuiRenderOnMainMenu()
{
#ifdef _DEBUG


	actor_manager->ImGuiRenderOnMainMenuBar();

	ImGui::SliderFloat("TimeLapseRatio", &time_lapse_ratio, 0.0f, 2.0f);

	if (ImGui::TreeNode("StageManager"))
	{
		if (ImGui::Checkbox("EditMode", &is_edit_mode))
		{
			if (is_edit_mode)
			{
				stage_manager->StartEditor();
			}
			else
			{
				stage_manager->EndEditor();
			}
		}

		stage_manager->ImGuiRenderOnMainMenuBar();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Minimap"))
	{

		hud_manager->GetMinimap().ImGuiRender();

		ImGui::TreePop();
	}

	// デバッグ用プリミティブの描画
	static bool show_debug_primitive_all{ false };
	if (ImGui::Checkbox("ShowDebugPrimitive", &show_debug_primitive_all))
	{
		for (const auto& a : actor_manager->GetUpdateActors())
		{
			for(const auto& c : a->GetColliders())
			{
				c.second->SetShowDebugPrimitive(show_debug_primitive_all);
			}
		}
	}
#endif // _DEBUG

}

void GameSystem::ImGuiRender()
{
#ifdef _DEBUG
	actor_manager->ImGuiRender();

	if(show_inventory_imgui) inventory->ImGuiRender();

	if (is_edit_mode)
	{
		stage_manager->ImGuiRender();
	}
#endif
}

void GameSystem::GameStart()
{
	TitleSystem::GetInstance().SetTitleFlag(false);
	hud_manager->SetShowHUDFlag(true);
}

void GameSystem::SetTimeLapseRatio(float ratio)
{
	time_lapse_ratio = ratio;	
}

void GameSystem::SetTimeLapseDelta(float delta, float dt)
{
	time_lapse_delta = delta;
	time_lapse_delta_t = dt;
}

void GameSystem::InitializeActor(ID3D11Device* device)
{
	const auto& actor_manager{ GameSystem::GetInstance().GetActorManager() };

	{
		auto player{ actor_manager->Create<ActorPlayer>() };
		player->Initialize(device, { 0, 0, 0 });

		// プレイヤーの持つ武器
		{
			auto weapon = player->CreateChildActor<ActorWeapon>();
			weapon->Initialize(
				device,
				"Sword",
				10,  // 攻撃力
				100, // 耐久値（未実装）
				{});
		}

		// プレイヤーを中心とした点光源
		{
			auto light = player->CreateChildActor<Actor>();
			constexpr Vector3 pos{ 0.0f, 2.0f, 0.0f };

			light->Initialize(
				"Light",
				pos,
				Quaternion::Identity(),
				{ 0, 0, 0 },
				"Light");

			light->AddComponent<PointLight>(
				Vector3(),
				14.0f,							// 光源の減衰
				Vector3(1.0f, 0.9f, 0.8f),		// 光源の色
				7.0f,							// 明るさ
				true);							// 全方位シャドウを行う点光源か
		}

		GameSystem::GetInstance().GetPlayerController()->SetPlayer(player);

		auto camera{ player->CreateChildActor<Actor>() };
		camera->Initialize(
			"MainCamera",
			{ 20.0f, 20.0f, -20.0f },
			Quaternion::Identity(),
			{ 1, 1, 1 },
			"Camera");
		camera->GetChildActorParam()->conform_position_only = true;

		auto camera_data{ std::make_shared<OrthographicCameraEX>(Vector3(0,0,0),Vector3(0,0,0),0.001f, 100.0f, true, 16.0f, 9.0f) };

		camera->AddComponent<ComponentCamera>(camera_data, true);
	}


	GameSystem::GetInstance().GetStageManager()->ChangeStage();
}
