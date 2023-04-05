#include "StageManager.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

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

/* --- �ϐ� ---- */

constexpr uint32_t initial_stage_index{ 0U };

/* --- ��` ---- */

StageManager::StageManager()
	: current_stage_index(), stage()
{
}

StageManager::~StageManager()
{
}

void StageManager::Initialize(ID3D11Device* device)
{
	// �ʏ�X�e�[�W�̃X�e�[�W�����f�[�^�̃L���b�V����j������B
	RemoveStageCacheFile();

	// �n�`�`��I�u�W�F�N�g�B�C���X�^���X�`��𗘗p�𗘗p���ē���̃��b�V���̒n�`���܂Ƃ߂ĕ`�悷��B
	terrain_renderer.Initialize(device);

	// �n�`�`��p�̃V�F�[�_�[�A�W�I���g���o�b�t�@�Ƀ��C�e�B���O�ɕK�v�ȏ��̏������݂��s���B
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

	//		// ���̃X�e�[�W�C���f�b�N�X�ɉ������X�e�[�W��ǂݍ��ށB
	//		// �ʏ�̃X�e�[�W���A��������Ă��Ȃ��X�e�[�W�̏ꍇ�͒n�`������������������B
	//		// ����ȊO�̏ꍇ�́A�t�@�C����L���b�V������n�`�f�[�^��ǂݍ��ށB
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
	// �C���X�^���X�`��
	terrain_renderer.Render(dc, shader ? shader : &terrain_shader, stage.GetTerrain());
}

void StageManager::EndProcess()
{
	// �ʏ�X�e�[�W�̃X�e�[�W�����f�[�^�̃L���b�V����j������B
	RemoveStageCacheFile();
}

void StageManager::Retry()
{
	// �����K�w
	next_stage_index = initial_stage_index;
}

void StageManager::ChangeStage(
	uint32_t stage_width, 
	uint32_t stage_height,
	uint32_t enemy_appearance_amount,
	uint32_t item_appearance_amount)
{
	const auto& actor_manager{ GameSystem::GetInstance().GetActorManager() };

	// ���݂̃X�e�[�W�̃A�N�^�Ƃ��Đ������ꂽ�I�u�W�F�N�g���Ǘ��N���X���珜�O����
	RemoveCurrentStageActors();

	// �X�e�[�W�̏�����
	stage.Initialize();

	switch (static_cast<ExtraStageIndex>(*next_stage_index))
	{
	case ExtraStageIndex::Tutorial:

		Tutorial::GetInstance().ResetFlag();
		Tutorial::GetInstance().SetTutorial(true);

		ChangeTutorialStage();

		// �X�e�[�W�̏�񂩂�G���e�B�e�B��z�u����B
		// �v���C���[�̈ʒu�������Ō��肷��
		stage.Enter();

		break;
	case ExtraStageIndex::Boss:

		Tutorial::GetInstance().SetTutorial(false);

		ChangeBossStage();

		// �X�e�[�W�̏�񂩂�G���e�B�e�B��z�u����B
		// �v���C���[�̈ʒu�������Ō��肷��
		stage.Enter(false); // �{�X�X�e�[�W�ł̓{�X��|���܂ŃG���A�ړ����ł��Ȃ�

		break;
	default:

		Tutorial::GetInstance().SetTutorial(false);

		ChangeCommonStage(
			stage_width,
			stage_height,
			enemy_appearance_amount,
			item_appearance_amount);

		// �X�e�[�W�̏�񂩂�G���e�B�e�B��z�u����B
		// �v���C���[�̈ʒu�������Ō��肷��
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

	// ���x���[������RigidBody���A�N�e�B�u��ԂłȂ��ꍇ�͏������s��Ȃ�
	if (rigid_body->GetVelocity().IsZero() ||
		rigid_body->IsActive() == false) return false;

	// �}�b�v�T�C�Y
	auto [column, row] { stage.GetStageSize() };

	// ���g�̂���ŋ߂̃^�C���C���f�b�N�X
	auto [c, r] { GetNearestIndex(actor) };

	constexpr int64_t judge_radius{ 1LL }; 

	// ���g�̎��͂̃}�X�̃^�C���̃C���f�b�N�X�A�z��͈̔͂Ɏ��߂�
	uint32_t cs{ static_cast<uint32_t>((std::clamp)(c - judge_radius, 0LL, static_cast<int64_t>(column))) };
	uint32_t ce{ static_cast<uint32_t>((std::clamp)(c + (judge_radius + 1LL), 0LL, static_cast<int64_t>(column))) };
	uint32_t rs{ static_cast<uint32_t>((std::clamp)(r - judge_radius, 0LL, static_cast<int64_t>(row))) };
	uint32_t re{ static_cast<uint32_t>((std::clamp)(r + (judge_radius + 1LL), 0LL, static_cast<int64_t>(row))) };

	const auto& terrain{ stage.GetTerrain() };

	constexpr float ray_l{ 0.5f };
	Vector3 ray_s{ actor->GetWorldPosition() };
	ray_s.y = 0.0f; // Y�͌v�Z�Ɋ܂߂Ȃ��i����Ɍv�Z�Ɋ܂܂�Ȃ��悤�ɂ��Ă����j
	Vector3 ray_d{ rigid_body->GetVelocity().Normalize() };

	// �X�t�B�A�L���X�g�̑O��AABB�Ƃ̔�������ĕ��׌y������
	Vector3 wall_norm{};
	float collide_near_len{ -1 };

	if (CollisionTerrainByAABB(rs, re, cs, ce, terrain.GetMap(), ray_s, ray_d, ray_l, wall_norm, collide_near_len))
	{
		// �Փ˓_�Ƃ̕ǂ���
		actor->GetRigidBody()->SlidingWall(
			ray_s,
			ray_s + ray_d * collide_near_len,
			wall_norm);

		return true;
	}

	// �X�t�B�A�L���X�g
	Vector3 ray_e{ ray_s + ray_d * ray_l };
	Vector3 intersect_pos;
	if (CollisionTerrainBySphereCast(rs, re, cs, ce, terrain, ray_s, ray_e, sphere_cast_radius, intersect_pos, wall_norm))
	{
		// �Փ˓_�Ƃ̕ǂ���
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
		// ���g�̂���ŋ߂̃^�C���C���f�b�N�X
		auto [c, r] { GetNearestIndex(actor.get()) };

		// �ǂ̔��肪�Â�������A�F�m���Ă��Ȃ��o�O������\��������̂ő΍􂵂Ă���
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
	ImGui::InputInt(TO_CHAR(u8"�ړ�����K�w"), &n);
	//static int32_t s[2]{ 20,20 };
	//ImGui::SliderInt2(TO_CHAR(u8"�V�����K�w�̃T�C�Y"), s, 20, 1000);

	if (ImGui::Button(TO_CHAR(u8"�ړ�����")))
	{
		GameSys.GetStageManager()->NextStage(static_cast<uint32_t>(n));
	}

	if (ImGui::Button(TO_CHAR(u8"�{�X�t���A")))
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
	// �w�肵���X�e�[�W�̃L���b�V�������݂���ꍇ�����ǂݍ���
	bool is_import{ ImportStage(*next_stage_index) };

	// �w�肵���X�e�[�W���܂���������Ă��Ȃ��ꍇ�A�X�e�[�W�𐶐�����B
	if (!is_import)
	{
		uint32_t w{ stage_width == 0U ? Random::GetValue(20U, 50U) : stage_width };
		uint32_t d{ stage_depth == 0U ? Random::GetValue(20U, 50U) : stage_depth };

		// �V�����K�w�̒n�`���쐬����
		stage.CreateTerrain(w, d, enemy_appearance_amount, item_appearance_amount);

		// ���������X�e�[�W�����o�͂���B
		stage.ExportStageData(*next_stage_index);
	}
}

bool StageManager::ImportStage(uint32_t stage_index)
{
	const std::filesystem::path name{ std::format("Data/StageData/Common/stage{}.bin", stage_index) };

	// �w�肵���p�X�����݂��Ȃ��ꍇ�A�n�`�f�[�^�̃L���b�V�������݂��Ȃ��B
	if (!std::filesystem::exists(name)) return false;

	// ���̓f�[�^����n�`�𐶐�����
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

			// �����蔻����s��Ȃ��^�C���B
			if (!(cell.data.collide & TileData::C_IsCollide)) continue;

			// ������s���ǂ̈ʒu(x, z), �T�C�Y(0.5, 0.5)
			Vector3 wall_pos{ static_cast<float>(ci) , 0.0f, static_cast<float>(ri) };

			float norm_x, norm_y;

			if (IntersectUser2D::IntersectRayVSAABB(
				ray_s.x, ray_s.z,
				ray_d.x, ray_d.z, ray_l,
				wall_pos.x - 0.5f, wall_pos.z - 0.5f, // �ǂ̒��_
				1.0f, 1.0f,							  // �ǂ̑傫���͂P
				out_near_len, norm_x, norm_y))
			{

				out_norm = { norm_x, 0.0f, norm_y };

				// ��������
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

	// ���g�̂���^�C���̎��͂̃^�C���Ƃ̂ݔ�������B
	for (uint32_t ri = rs; ri < re; ++ri)
	{
		for (uint32_t ci = cs; ci < ce; ++ci)
		{
			const auto& cell{ terrain.GetMap()[ci][ri] };

			// �����蔻����s��Ȃ��^�C���B
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


