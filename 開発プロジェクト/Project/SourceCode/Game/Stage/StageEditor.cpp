#include "StageEditor.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"
#include "General/MyImGuiHelper.h"

#include "Arithmetic/MyMath.h"
#include "Debug/DebugPrimitive.h"
#include "General/AtaiEngine.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Camera/OrthographicCameraEX.h"
#include "Graphics/PostProcess/DeferredLighting.h"
#include "Input/Input.h"
#include "Utility/FileDialogBox.h"

#include "StageData.h"
#include "Terrain/TerrainGenerator.h"

/* --- 定義 ---- */

StageEditor::StageEditor()
{
}

StageEditor::~StageEditor()
{
}

void StageEditor::Start()
{
	// カメラの取得
	if (auto frame_camera = 
		std::dynamic_pointer_cast<OrthographicCameraEX>(
			Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera()))
	{
		tmp_camera = frame_camera;
		camera = std::make_shared<OrthographicCamera>(*frame_camera);
	}

	// メインカメラに設定
	Graphics::GetInstance().GetFrameBuffer()->PrepareCamera(camera);

	light = std::make_shared<DeferredLighting::Light>();
	light->light_color = { 0.7f, 0.7f, 0.7f };
	light->light_parameter = { 0.0f, 1.0f, -1.0f, 0.0f };
	light->luminance = 5.0f;
	Graphics::GetInstance().GetDeferredLighting()->RegisterLight(light);
}

void StageEditor::End()
{
	// カメラを元に戻す
	Graphics::GetInstance().GetFrameBuffer()->PrepareCamera(tmp_camera);

	// 光源を破棄
	light.reset();
}

void StageEditor::Update(float elapsed_time, Stage& out_stage)
{
	ContorlCamera(elapsed_time);

	MarkEntitySpawnPoint(out_stage);
}

void StageEditor::ImGuiRender(Stage& out_stage)
{
	ImGui::Begin("StageEditor", 0, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();

	if (ImGui::BeginMenu("New"))
	{
		static int s[2]{ 30 , 30 };
		ImGui::SliderInt2("Size", s, 1, 100);
		if (ImGui::MenuItem("Create"))
		{
			Create(static_cast<uint32_t>(s[0]), static_cast<uint32_t>(s[1]), out_stage);
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Import")) 
		{
			Import(out_stage); 
			TerrainGenerator::UpdateTile(out_stage.GetTerrain());
		};
		if (ImGui::MenuItem("Export")) 
		{
			TerrainGenerator::UpdateTile(out_stage.GetTerrain());
			Export(out_stage); 
		};

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();

	if (ImGui::TreeNode("Light"))
	{
		ImGui::SliderFloat3("Dir", &light->light_parameter.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Lum", &light->luminance, 0.0f, 100.0f);
		ImGui::ColorEdit3("Col", &light->light_color.x);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("EditCell"))
	{
		std::vector<std::u8string> mode_list
		{
			u8"右クリックしたセルのフラグを確認",
			u8"右クリック中にカーソルを合わせたセルに設定したフラグを立てる"
		};

		static int mode{ 0 };

		ImGui::ListBox(TO_CHAR(u8"モード"), &mode,
			[](void* data, int idx, const char** out_text) -> bool
			{
				auto list{ reinterpret_cast<std::vector<std::u8string>*>(data) };
				(*out_text) = TO_CHAR((*list)[idx].c_str());
				return true;
			}, &mode_list, static_cast<int>(mode_list.size()));

		switch (mode)
		{
		case 0:
			CheckCell(out_stage);
			break;
		case 1:
			ChangeCell(out_stage);
			break;
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void StageEditor::ContorlCamera(float elapsed_time)
{
	if (!camera) return;

	const auto& cursor{ Input::GetInstance().GetCursor() };
	const auto& gamepad{ Input::GetInstance().GetGamePad() };
	const auto& keyboard{ Input::GetInstance().GetKeyboard() };
	const float wheel{ Input::GetInstance().GetVerticalWheelState() };

	const auto& cp{ cursor.GetCurrentPosition() };
	const auto& dp{ cursor.GetPreviousPosition() };

	const Vector3& right{ camera->GetRight() };
	const Vector3& up{ camera->GetUp() };
	const Vector3& forward{ camera->GetForward() };

	Vector3 move_val{};
	Vector3 focus{ camera->GetFocus() };
	Vector3 eye{ camera->GetEye() };

	if (keyboard.GetKeyState(VK::MBUTTON) & Keyboard::On)
	{
		if (keyboard.GetKeyState(VK::LSHIFT) & Keyboard::On)
		{
			focus += right * static_cast<float>(cp.x - dp.x) * elapsed_time;
			focus -= up * static_cast<float>(cp.y - dp.y) * elapsed_time;
			move_val += right * static_cast<float>(cp.x - dp.x) * elapsed_time;
			move_val -= up * static_cast<float>(cp.y - dp.y) * elapsed_time;
		}
		//else
		//{
		//	move_val -= right * static_cast<float>(cp.x - dp.x) * elapsed_time;
		//	move_val += up * static_cast<float>(cp.y - dp.y) * elapsed_time;
		//}
	}

	if (wheel > FLT_EPSILON || wheel < -FLT_EPSILON)
	{
		float ratio{ 9.0f / 16.0f };

		camera->SetWidth((std::max)(camera->GetWidth() + wheel * elapsed_time, 1.0f));
		camera->SetHeight((std::max)(camera->GetHeight() + wheel * ratio * elapsed_time, ratio));

		
	}

	eye += move_val;

	camera->SetEye(eye);
	camera->SetFocus(focus);

	camera->Update(elapsed_time);
}

void StageEditor::Create(uint32_t width, uint32_t height, Stage& out_stage)
{
	out_stage.CreateTerrain(width, height, 0, 0);
}

void StageEditor::Export(const Stage& stage)
{
	auto path{ FileDialogBox::SelectPathA(FileDialogBox::Save, "*.bin\0*.bin\0", ".\\Data\\Stage\\") };
	path += ".bin";
	StageData export_data;


	export_data.Export(path.c_str(), stage.GetTerrain());

}

void StageEditor::Import(Stage& out_stage)
{
	auto path{ FileDialogBox::SelectPathA(FileDialogBox::Open, "*.bin\0*.bin\0", ".\\Data\\Stage\\") };
	out_stage.CreateTerrain(path.c_str());

	TerrainGenerator::UpdateTile(out_stage.GetTerrain());
}

void StageEditor::MarkEntitySpawnPoint(const Stage& stage)
{
	const auto& terrain{ stage.GetTerrain() };

	auto draw_mark = [](uint32_t ci, uint32_t ri, const Vector4& color)
	{
		DebugPrimitive::GetInstance().DrawCube(
			{ static_cast<float>(ci), 0.0f, static_cast<float>(ri) },
			{ 0.25f, 0.0f, 0.25f },
			color);
	};

	auto enemies{ terrain.FindMapIndices(TileData::S_EnemySpawn) };
	for(const auto& [ci, ri] : enemies)
	{
		draw_mark(ci, ri, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	auto items{ terrain.FindMapIndices(TileData::S_ItemSpawn) };
	for(const auto& [ci, ri] : items)
	{
		draw_mark(ci, ri, { 0.0f, 1.0f, 0.0f, 1.0f });
	}
}

void StageEditor::SelectCell(const Stage& stage)
{
	const auto& input{ Input::GetInstance() };

	// カメラの取得
	if (auto camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera())
	{
		auto [width, height] { MySystem.GetScreenSize<float>() };
		POINT scr_pos{ input.GetCursor().GetCurrentPosition() };

		auto [w_near, w_far] { Math::ScreenToWorld(
			Vector2(static_cast<float>(scr_pos.x), static_cast<float>(scr_pos.y)),
			camera->GetView(),
			camera->GetProjection(),
			width, height) };


		// y = 0 の地点を算出する
		Vector3 ray{ w_far - w_near };
		ray = ray.Normalize();

		// レイが下向きか
		if (ray.y <= 0)
		{
			constexpr Vector3 face{ 0.0f, 1.0f, 0.0f };

			Vector3 intersect_pos{};
			// 始点から原点への距離
			float len0{ (-w_near).Dot(face) };
			// 始点から終点への距離
			float len_r{ ray.Dot(face) };

			// 二つの距離の比率からレイの平面までのベクトルを算出する
			intersect_pos = w_near + (len0 / len_r) * ray;

			int64_t ci{ static_cast<int64_t>(std::round(intersect_pos.x)) };
			int64_t ri{ static_cast<int64_t>(std::round(intersect_pos.z)) };

			auto [w, h] { stage.GetStageSize() };

			ci = std::clamp(ci, 0LL, static_cast<int64_t>(w) - 1LL);
			ri = std::clamp(ri, 0LL, static_cast<int64_t>(h) - 1LL);

#ifdef _DEBUG
			DebugPrimitive::GetInstance().DrawCube(
				Vector3(static_cast<float>(ci), 0.0f, static_cast<float>(ri)),
				Vector3(0.5f, 0.0f, 0.5f),
				Vector4(1, 1, 1, 1));
#endif // _DEBUG

			if (input.GetKeyboard().GetKeyState(VK::RBUTTON) & Keyboard::On)
			{
				// 交差した位置のセルのインデックス(小数四捨五入)
				selecting_cell.first =  ci;
				selecting_cell.second = ri;
			}
		}
	}

}

void StageEditor::CheckCell(const Stage& out_stage)
{
	auto [w, h] { out_stage.GetStageSize() };

	// 右クリックした先にあるセルを選択
	SelectCell(out_stage);

	auto [ci, ri] { selecting_cell };

	if (ci < 0LL || ri < 0LL || ci > static_cast<uint32_t>(w) || ri > static_cast<uint32_t>(h))
	{
		ImGui::Text(TO_CHAR(u8"セルが選択されていません"));
		return;
	}

	const auto& tile{ out_stage.GetTerrain().GetMap()[ci][ri] };

	auto func = [tile](uint32_t s) -> bool
	{
		return (tile.data.state == s);
	};

	bool is_floor{ tile.IsFloor() };

	ImGui::RadioButton("Floor", is_floor);
	ImGui::SameLine();
	ImGui::RadioButton("Wall", !is_floor);
	

	if (is_floor)
	{
		ImGui::RadioButton("EnemySpawn", tile.data.state == TileData::S_EnemySpawn);
		ImGui::SameLine();
		ImGui::RadioButton("PlayerSpawn", tile.data.state == TileData::S_PlayerSpawn);
		ImGui::SameLine();
		ImGui::RadioButton("ItemSpawn", tile.data.state == TileData::S_ItemSpawn);
		ImGui::SameLine();
		ImGui::RadioButton("NextFloor", tile.data.state == TileData::S_NextFloor);

		switch (tile.data.state)
		{
		case TileData::S_EnemySpawn:
			ImGui::RadioButton("Random", tile.data.entity == TileData::E_Random);
			ImGui::SameLine();
			ImGui::RadioButton("Skeleton", tile.data.entity == TileData::E_Skeleton);
			ImGui::SameLine();
			ImGui::RadioButton("Wolf", tile.data.entity == TileData::E_Wolf);
			ImGui::SameLine();
			ImGui::RadioButton("Spider", tile.data.entity == TileData::E_Spider);
			ImGui::SameLine();
			ImGui::RadioButton("Boss", tile.data.entity == TileData::E_Boss);

			ImGui::RadioButton("HasWeapon", tile.data.other & TileData::O_EnemyHasWeapon);
			break;
		case TileData::S_ItemSpawn:

			ImGui::RadioButton("Random", tile.data.entity == TileData::I_Random);
			ImGui::SameLine();
			ImGui::RadioButton("LifePot", tile.data.entity == TileData::I_LifePot);
			ImGui::SameLine();
			ImGui::RadioButton("FirePot", tile.data.entity == TileData::I_FirePot);
			ImGui::SameLine();
			ImGui::RadioButton("SparkPot", tile.data.entity == TileData::I_SparkPot);
			ImGui::SameLine();
			ImGui::RadioButton("PoisonPot", tile.data.entity == TileData::I_PoisonPot);

			break;
		}
	}
	// 壁の時
	else
	{
		ImGui::RadioButton("CollideWall", tile.data.collide == TileData::C_IsCollide);
	}
}

void StageEditor::ChangeCell(Stage& out_stage)
{
	auto [w, h] { out_stage.GetStageSize() };

	// クリック下セルに付与するステート

	enum class Type { Floor, Wall };
	static Type type{ Type::Floor };

	if (ImGui::RadioButton("Floor", type == Type::Floor)) { type = Type::Floor; }
	ImGui::SameLine();
	if (ImGui::RadioButton("Wall", type == Type::Wall)) { type = Type::Wall; }

	static TileData::State state{ TileData::S_None };
	static TileData::EntityID entity_id{ TileData::E_None };
	static TileData::OtherFlag other{ TileData::O_None };
	static TileData::CollideFlag collide{ TileData::C_None };

	switch (type)
	{
	case Type::Floor:
	{
		auto radio_state = [&](const char* label, TileData::State s)
		{
			bool b{ state == s };
			if (ImGui::RadioButton(label, b))
			{
				if (b)
				{
					state = TileData::S_None;
				}
				else
				{
					state = s;
				}
			}
		};

		radio_state("EnemySpawn", TileData::S_EnemySpawn);
		ImGui::SameLine();
		radio_state("PlayerSpawn", TileData::S_PlayerSpawn);
		ImGui::SameLine();
		radio_state("ItemSpawn", TileData::S_ItemSpawn);
		ImGui::SameLine();
		radio_state("NextFloor", TileData::S_NextFloor);

		switch (state)
		{
		case TileData::S_EnemySpawn:
		{

			auto radio_enemy = [&](const char* label, TileData::EntityID e)
			{
				bool b{ entity_id == e };
				if (ImGui::RadioButton(label, b))
				{
					if (b)
					{
						entity_id = TileData::E_Random;
					}
					else
					{
						entity_id = e;
					}
				}
			};

			radio_enemy("Random", TileData::E_Random);
			ImGui::SameLine();
			radio_enemy("Skeleton", TileData::E_Skeleton);
			ImGui::SameLine();
			radio_enemy("Wolf", TileData::E_Wolf);
			ImGui::SameLine();
			radio_enemy("Spider", TileData::E_Spider);
			ImGui::SameLine();
			radio_enemy("Boss", TileData::E_Boss);

			if (entity_id == TileData::E_Skeleton)
			{
				bool b{ (other & TileData::O_EnemyHasWeapon) > 0 };
				if (ImGui::RadioButton("HasWeapon", b))
				{
					if (b)
					{
						other = static_cast<TileData::OtherFlag>(other & ~TileData::O_EnemyHasWeapon);
					}
					else
					{
						other = static_cast<TileData::OtherFlag>(other | TileData::O_EnemyHasWeapon);
					}
				}
			}
			else
			{
				other = static_cast<TileData::OtherFlag>(other & ~TileData::O_EnemyHasWeapon);
			}

			collide = TileData::C_None;

			break;
		}
		case TileData::S_ItemSpawn:
		{
			auto radio_item = [&](const char* label, TileData::EntityID i)
			{
				bool b{ entity_id == i };
				if (ImGui::RadioButton(label, b))
				{
					if (b)
					{
						entity_id = TileData::I_Random;
					}
					else
					{
						entity_id = i;
					}
				}
			};

			radio_item("Random", TileData::I_Random);
			ImGui::SameLine();
			radio_item("LifePot", TileData::I_LifePot);
			ImGui::SameLine();
			radio_item("FirePot", TileData::I_FirePot);
			ImGui::SameLine();
			radio_item("SparkPot", TileData::I_SparkPot);
			ImGui::SameLine();
			radio_item("PoisonPot", TileData::I_PoisonPot);

			other = static_cast<TileData::OtherFlag>(other & ~TileData::O_EnemyHasWeapon);
			collide = TileData::C_None;

			break;
		}
		default:
			
			entity_id = TileData::E_None;
			other = static_cast<TileData::OtherFlag>(other & ~TileData::O_EnemyHasWeapon);
			collide = TileData::C_None;

		}

		break;
	}
	case Type::Wall:
	{
		// 右クリックした先にあるセルを選択
		SelectCell(out_stage);

		auto [ci, ri] { selecting_cell };

		if (ci < 0LL || ri < 0LL || ci > static_cast<uint32_t>(w) || ri > static_cast<uint32_t>(h))
		{
			return;
		}

		auto& tile{ out_stage.GetTerrain().GetMap()[ci][ri] };

		state = TileData::S_None;
		entity_id = TileData::E_None;
		other = static_cast<TileData::OtherFlag>(other & ~TileData::O_EnemyHasWeapon);
		collide = TileData::C_IsCollide;

	}
	}


	// そのセルの情報の更新
	
	// 右クリックした先にあるセルを選択
	SelectCell(out_stage);

	auto [ci, ri] { selecting_cell };

	if (ci < 0LL || ri < 0LL || ci > static_cast<uint32_t>(w) || ri > static_cast<uint32_t>(h))
	{
		return;
	}

	auto& tile{ out_stage.GetTerrain().GetMap()[ci][ri] };

	tile.data.mesh = type == Type::Floor ? TileData::MeshID::M_Floor : TileData::MeshID::M_Wall;
	tile.data.state = state;
	tile.data.entity = entity_id;
	tile.data.other = other;
	tile.data.collide = collide;

	TerrainGenerator::UpdateTileMaterial(tile);
	TerrainGenerator::UpdateTileCollide(out_stage.GetTerrain().GetMap(), ci, ri, static_cast<int64_t>(w), static_cast<int64_t>(h));


	// 選択中のセルを解除
	selecting_cell = { -1, -1 };


}
