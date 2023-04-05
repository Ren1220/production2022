#include "StageManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <filesystem>

#include "imgui.h"

#include "Game/Actor/Actor.h"
#include "Game/General/GameSystem.h"
#include "Game/General/Tutorial.h"
#include "Game/General/SceneFade.h"
#include "Game/Physics/RigidBody.h"
#include "General/MyImGuiHelper.h"
#include "IntersectUser/Intersect2D.h"
#include "IntersectUser/IntersectModel.h"
#include "IntersectUser/IntersectUserEX.h"
#include "Terrain/Terrain.h"
#include "Utility/RandomFunction.h"

/* --- 変数 ---- */

constexpr uint32_t initial_stage_index{ 0U };

/* --- 定義 ---- */

StageManager::StageManager()
	: current_stage_index(), stage()
{
}

StageManager::~StageManager()
{
}

void StageManager::Initialize(ID3D11Device* device)
{
	// 通常ステージのステージ生成データのキャッシュを破棄する。
	RemoveStageCacheFile();

	// 地形描画オブジェクト。インスタンス描画を利用を利用して同一のメッシュの地形をまとめて描画する。
	terrain_renderer.Initialize(device);

	// 地形描画用のシェーダー、ジオメトリバッファにライティングに必要な情報の書き込みを行う。
	terrain_shader.CreateBlendState(device, BSMode::Alpha);
	terrain_shader.CreateVSFromCSO(device, "Shader/GeometryBufferInstancedVS.cso");
	//terrain_shader.CreateHSFromCSO(device, "Shader/GeometryBufferInstancedHS.cso");
	//terrain_shader.CreateDSFromCSO(device, "Shader/GeometryBufferInstancedDS.cso");
	terrain_shader.CreatePSFromCSO(device, "Shader/GeometryBufferInstancedPS.cso");
	terrain_shader.CreateSamplerState(device, 0, D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	terrain_shader.CreateSamplerState(device, 1, D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	terrain_shader.CreateSamplerState(device, 2, D3D11_FILTER_MINIMUM_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);

	stage.Initialize();
	next_stage_index = initial_stage_index;
}

void StageManager::Update(float elapsed_time)
{
	//if (next_stage_index)
	//{
	//	if (current_stage_index != *next_stage_index)
	//	{
	//		GameSys.

	//		// 次のステージインデックスに応じたステージを読み込む。
	//		// 通常のステージかつ、生成されていないステージの場合は地形生成処理が発生する。
	//		// それ以外の場合は、ファイルやキャッシュから地形データを読み込む。
	//		ChangeStage(); 
	//	}
	//}
}

void StageManager::UpdateEditor(float elapsed_time)
{
#ifdef _DEBUG
	editor.Update(elapsed_time, stage);
#endif // _DEBUG
}

void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// インスタンス描画
	terrain_renderer.Render(dc, shader ? shader : &terrain_shader, stage.GetTerrain());
}

void StageManager::EndProcess()
{
	// 通常ステージのステージ生成データのキャッシュを破棄する。
	RemoveStageCacheFile();
}

void StageManager::Retry()
{
	// 初期階層
	next_stage_index = initial_stage_index;
}

void StageManager::ChangeStage(
	uint32_t stage_width, 
	uint32_t stage_height,
	uint32_t enemy_appearance_amount,
	uint32_t item_appearance_amount)
{
	const auto& actor_manager{ GameSystem::GetInstance().GetActorManager() };

	// 現在のステージのアクタとして生成されたオブジェクトを管理クラスから除外する
	RemoveCurrentStageActors();

	// ステージの初期化
	stage.Initialize();

	switch (static_cast<ExtraStageIndex>(*next_stage_index))
	{
	case ExtraStageIndex::Tutorial:

		Tutorial::GetInstance().ResetFlag();
		Tutorial::GetInstance().SetTutorial(true);

		ChangeTutorialStage();

		// ステージの情報からエンティティを配置する。
		// プレイヤーの位置もここで決定する
		stage.Enter();

		break;
	case ExtraStageIndex::Boss:

		Tutorial::GetInstance().SetTutorial(false);

		ChangeBossStage();

		// ステージの情報からエンティティを配置する。
		// プレイヤーの位置もここで決定する
		stage.Enter(false); // ボスステージではボスを倒すまでエリア移動ができない

		break;
	default:

		Tutorial::GetInstance().SetTutorial(false);

		ChangeCommonStage(
			stage_width,
			stage_height,
			enemy_appearance_amount,
			item_appearance_amount);

		// ステージの情報からエンティティを配置する。
		// プレイヤーの位置もここで決定する
		stage.Enter();

		break;
	}

	current_stage_index = *next_stage_index;
	next_stage_index.reset();
}

bool StageManager::IntersectModelVSTerrain(
	Actor* actor, 
	float sphere_cast_radius)
{
	auto rigid_body{ actor->GetRigidBody() };
	if (!rigid_body)
	{
		return false;
	}

	// 速度がゼロ又はRigidBodyがアクティブ状態でない場合は処理を行わない
	if (rigid_body->GetVelocity().IsZero() ||
		rigid_body->IsActive() == false) return false;

	// マップサイズ
	auto [column, row] { stage.GetStageSize() };

	// 自身のいる最近のタイルインデックス
	auto [c, r] { GetNearestIndex(actor) };

	constexpr int64_t judge_radius{ 1LL }; 

	// 自身の周囲のマスのタイルのインデックス、配列の範囲に収める
	uint32_t cs{ static_cast<uint32_t>((std::clamp)(c - judge_radius, 0LL, static_cast<int64_t>(column))) };
	uint32_t ce{ static_cast<uint32_t>((std::clamp)(c + (judge_radius + 1LL), 0LL, static_cast<int64_t>(column))) };
	uint32_t rs{ static_cast<uint32_t>((std::clamp)(r - judge_radius, 0LL, static_cast<int64_t>(row))) };
	uint32_t re{ static_cast<uint32_t>((std::clamp)(r + (judge_radius + 1LL), 0LL, static_cast<int64_t>(row))) };

	const auto& terrain{ stage.GetTerrain() };

	constexpr float ray_l{ 0.5f };
	Vector3 ray_s{ actor->GetWorldPosition() };
	ray_s.y = 0.0f; // Yは計算に含めない（下手に計算に含まれないようにしておく）
	Vector3 ray_d{ rigid_body->GetVelocity().Normalize() };

	// スフィアキャストの前にAABBとの判定をして負荷軽減する
	Vector3 wall_norm{};
	float collide_near_len{ -1 };

	if (CollisionTerrainByAABB(rs, re, cs, ce, terrain.GetMap(), ray_s, ray_d, ray_l, wall_norm, collide_near_len))
	{
		// 衝突点との壁ずり
		actor->GetRigidBody()->SlidingWall(
			ray_s,
			ray_s + ray_d * collide_near_len,
			wall_norm);

		return true;
	}

	// スフィアキャスト
	Vector3 ray_e{ ray_s + ray_d * ray_l };
	Vector3 intersect_pos;
	if (CollisionTerrainBySphereCast(rs, re, cs, ce, terrain, ray_s, ray_e, sphere_cast_radius, intersect_pos, wall_norm))
	{
		// 衝突点との壁ずり
		actor->GetRigidBody()->SlidingWall(
			ray_s,
			intersect_pos,
			wall_norm);

		return true;
	}

	return false;
}

bool StageManager::ExecuteFloorEvent()
{
	bool next_stage_flag{ false };

	for (const auto& actor : GameSys.GetActorManager()->GetUpdateActors())
	{
		// 自身のいる最近のタイルインデックス
		auto [c, r] { GetNearestIndex(actor.get()) };

		// 壁の判定が甘かったり、認知していないバグがある可能性もあるので対策しておく
		{
			auto [column, row] { GetStage().GetStageSize() };
			if (c < 0 || c >= column || r < 0 || r >= row) continue;
		}

		const auto& tile{ GetStage().GetTerrain().GetMap()[c][r] };

		if (stage.CanNext())
		{
			if (tile.data.state == TileData::S_NextFloor)
			{
				if (actor->CompareGroupName("Player"))
				{
					next_stage_flag = true;
				}
			}
		}
	}

	return next_stage_flag;
}

std::pair<int64_t, int64_t> StageManager::GetNearestIndex(Actor* actor)
{
	const Vector3 ap{ actor->GetWorldPosition() };

	int64_t rx{ static_cast<int64_t>(round(ap.x)) };
	int64_t rz{ static_cast<int64_t>(round(ap.z)) };

	return { rx , rz };
}

void StageManager::ImGuiRenderOnMainMenuBar()
{
	static int n{ 0 };
	n = (std::max)(0, n);
	ImGui::InputInt(TO_CHAR(u8"移動する階層"), &n);
	//static int32_t s[2]{ 20,20 };
	//ImGui::SliderInt2(TO_CHAR(u8"新しい階層のサイズ"), s, 20, 1000);

	if (ImGui::Button(TO_CHAR(u8"移動する")))
	{
		GameSys.GetStageManager()->NextStage(static_cast<uint32_t>(n));
	}

	if (ImGui::Button(TO_CHAR(u8"ボスフロア")))
	{
		GameSys.GetStageManager()->NextStage(static_cast<uint32_t>(ExtraStageIndex::Boss));
	}

	// stage.ImGuiRender();
}

void StageManager::ImGuiRender()
{
#ifdef _DEBUG
	editor.ImGuiRender(stage);
#endif // _DEBUG
}

void StageManager::NextStage(const std::function<uint32_t(uint32_t)>& func, float fade_delta)
{
	if (func)
	{
		next_stage_index = func(current_stage_index);
	}
	else
	{
		next_stage_index = current_stage_index + 1U;
	}

	GameSys.SetTransitionStageFlag(true);
	AreaChange::GetInstance().FadeIn(fade_delta);
}

void StageManager::NextStage(uint32_t i, float fade_delta)
{
	next_stage_index = i;
	GameSys.SetTransitionStageFlag(true);
	AreaChange::GetInstance().FadeIn(fade_delta);
}

void StageManager::StartEditor()
{
#ifdef _DEBUG
	editor.Start();
#endif // _DEBUG

}

void StageManager::EndEditor()
{
#ifdef _DEBUG
	editor.End();
#endif // _DEBUG
}

void StageManager::RegisterActor(const std::shared_ptr<Actor>& actor)
{
	stage.RegisterActor(actor);
}

void StageManager::ActivateWarp()
{
	stage.ActivateWarp();
}

void StageManager::ChangeTutorialStage()
{
	using namespace std::string_view_literals;
	auto path{ "Data/StageData/Extra/tutorial.bin"sv };

	stage.CreateTerrain(path.data());
}

void StageManager::ChangeBossStage()
{
	using namespace std::string_view_literals;
	auto path{ "Data/StageData/Extra/boss.bin"sv };

	stage.CreateTerrain(path.data());
}

void StageManager::ChangeCommonStage(
	uint32_t stage_width, 
	uint32_t stage_depth,
	uint32_t enemy_appearance_amount, 
	uint32_t item_appearance_amount)
{
	// 指定したステージのキャッシュが存在する場合それを読み込む
	bool is_import{ ImportStage(*next_stage_index) };

	// 指定したステージがまだ生成されていない場合、ステージを生成する。
	if (!is_import)
	{
		uint32_t w{ stage_width == 0U ? Random::GetValue(20U, 50U) : stage_width };
		uint32_t d{ stage_depth == 0U ? Random::GetValue(20U, 50U) : stage_depth };

		// 新しい階層の地形を作成する
		stage.CreateTerrain(w, d, enemy_appearance_amount, item_appearance_amount);

		// 生成したステージ情報を出力する。
		stage.ExportStageData(*next_stage_index);
	}
}

bool StageManager::ImportStage(uint32_t stage_index)
{
	const std::filesystem::path name{ std::format("Data/StageData/Common/stage{}.bin", stage_index) };

	// 指定したパスが存在しない場合、地形データのキャッシュが存在しない。
	if (!std::filesystem::exists(name)) return false;

	// 入力データから地形を生成する
	stage.CreateTerrain(name.string().c_str());

	return true;
}

void StageManager::RemoveStageCacheFile()
{
	const std::filesystem::path stage_data_dir{ "Data/StageData/Common" };

	std::filesystem::remove_all(stage_data_dir);
}

bool StageManager::CollisionTerrainByAABB(
	uint32_t rs, uint32_t re, 
	uint32_t cs, uint32_t ce, 
	const std::vector<std::vector<TerrainTile>>& map,
	const Vector3& ray_s,
	const Vector3& ray_d, 
	float ray_l,
	Vector3& out_norm, float& out_near_len)
{
	for (uint32_t ri = rs; ri < re; ++ri)
	{
		for (uint32_t ci = cs; ci < ce; ++ci)
		{
			const auto& cell{ map[ci][ri] };

			// 当たり判定を行わないタイル。
			if (!(cell.data.collide & TileData::C_IsCollide)) continue;

			// 判定を行う壁の位置(x, z), サイズ(0.5, 0.5)
			Vector3 wall_pos{ static_cast<float>(ci) , 0.0f, static_cast<float>(ri) };

			float norm_x, norm_y;

			if (IntersectUser2D::IntersectRayVSAABB(
				ray_s.x, ray_s.z,
				ray_d.x, ray_d.z, ray_l,
				wall_pos.x - 0.5f, wall_pos.z - 0.5f, // 壁の頂点
				1.0f, 1.0f,							  // 壁の大きさは１
				out_near_len, norm_x, norm_y))
			{

				out_norm = { norm_x, 0.0f, norm_y };

				// 交差した
				return true;
			}
		}
	}

	return false;
}

bool StageManager::CollisionTerrainBySphereCast(
	uint32_t rs, uint32_t re, 
	uint32_t cs, uint32_t ce, 
	const Terrain& terrain,
	const Vector3& ray_s,
	const Vector3& ray_e,
	float sphere_radius,
	Vector3& out_intersect_pos,
	Vector3& out_wall_norm)
{
	IntersectUser::HitResult::ResultSphereCastVSModel result{};
	const auto& wall_mesh{ terrain_renderer.GetWallMesh() };

	// 自身のいるタイルの周囲のタイルとのみ判定を取る。
	for (uint32_t ri = rs; ri < re; ++ri)
	{
		for (uint32_t ci = cs; ci < ce; ++ci)
		{
			const auto& cell{ terrain.GetMap()[ci][ri] };

			// 当たり判定を行わないタイル。
			if (!(cell.data.collide & TileData::C_IsCollide)) continue;

			Matrix world{ terrain.CalcWorldTransform(ci, ri) };

			//if (IntersectUser::IntersectRayCastVSModel(
			//	s, e, mesh.model.get(), world, result))
			if (IntersectUser::IntersectSphereCastVSModelEX(
				ray_s, ray_e, sphere_radius, wall_mesh.GetModel(), world, cell.data.collide, result))
			{
				out_intersect_pos = result.intersect_position;
				out_wall_norm = result.triangle_normal;

				return true;
			}
		}
	}

	return false;
}

void StageManager::RemoveCurrentStageActors()
{
	GameSys.GetActorManager()->Remove(stage.GetStageActors());
}


