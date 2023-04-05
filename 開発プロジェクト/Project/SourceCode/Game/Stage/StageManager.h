#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <optional>

#include "Graphics/Shader/Shader.h"

#include "Stage.h"
#include "StageData.h"
#include "StageEditor.h"
#include "Terrain/TerrainRenderer.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class StageManager
{
public:

	/// <summary>
	/// �����Őݒ肵���C���f�b�N�X�̊K�w�̃X�e�[�W�͕ʏ����ŃX�e�[�W��ǂݍ��݁A��������
	/// </summary>
	enum class ExtraStageIndex : uint32_t
	{
		Tutorial = 0U, // �[���K�w�ڂ̓`���[�g���A��
		Boss = 4U, //�@�܂��{�X�͖�����
	};

	StageManager();
	~StageManager();

	void Initialize(ID3D11Device* device);
	void Update(float elapsed_time);
	void UpdateEditor(float elapsed_time);
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr);
	void EndProcess();
	void ImGuiRenderOnMainMenuBar();
	void ImGuiRender();

	void Retry();

	/// <summary>
	/// �����̃C���f�b�N�X�̊K�w�Ɉړ�����B�v���C���[�̈ʒu�����肷��B
	/// ���̊K�w�̃G���e�B�e�B�Ƃ��ēo�^���ꂽ�A�N�^�͂��ׂăA�N�^�}�l�[�W���[����폜�����B
	/// ���߂Ĉړ�����K�w�̏ꍇ�A�n�`�������s���A���������n�`�f�[�^���L���b�V���f�[�^�Ƃ��ďo�͂���B
	/// ��x�ł��ړ������K�w�̏ꍇ�́A�L���b�V���f�[�^����n�`��ǂݍ��ށB
	/// �L���A���s�����O�ɐݒ肵�Ă���ꍇ 20~100 �͈̔͂Ŋe�����_���ɐݒ肳���B
	/// </summary>
	void ChangeStage(
		uint32_t stage_width = 0U, 
		uint32_t stage_height = 0U,
		uint32_t enemy_appearance_amount = 5U,
		uint32_t item_appearance_amount = 5U);

	/// <summary>
	/// �������ꂽ�n�`�Ƃ̓����蔻��B
	/// ����X�t�B�A�L���X�g�ł̔���B
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="sphere_cast_radius"></param>
	/// <returns></returns>
	bool IntersectModelVSTerrain(Actor* actor, float sphere_cast_radius);

	/// <summary>
	/// �����̃A�N�^�̉��̏��ɉ������C�x���g�����s����B
	/// �t���A�ړ������v���C�������񂾏ꍇtrue��Ԃ�
	/// </summary>
	bool ExecuteFloorEvent();

	/// <summary>
	/// �����̃A�N�^�̍��W��X,Z���l�̌ܓ������l���Ŋ��̃^�C���Ɣ��f���Ă��̒l��Ԃ��B
	/// </summary>
	/// <param name="actor"></param>
	/// <returns>column, row</returns>
	std::pair<int64_t, int64_t> GetNearestIndex(Actor* actor);


	const Stage& GetStage() const { return stage; }

	uint32_t GetCurrentStageIndex() const  { return current_stage_index; }
	void	 SetNextStageIndex(uint32_t i) { next_stage_index = i;       }

	/// <summary>
	/// ���̃X�e�[�W�̃C���f�b�N�X���w�肵�A�X�e�[�W�J�ڃt���O�𗧂Ă�B
	/// </summary>
	/// <param name="func">nullptr�̏ꍇ�A���̃X�e�[�W�̃C���f�b�N�X�͌��݂̃C���f�b�N�X�{�P</param>
	/// <param name="fade_delta">nullptr�̏ꍇ�A���̃X�e�[�W�̃C���f�b�N�X�͌��݂̃C���f�b�N�X�{�P</param>
	void NextStage(const std::function<uint32_t(uint32_t)>& func = nullptr, float fade_delta = 2.0f);
	
	/// <summary>
	/// ���̃X�e�[�W�̃C���f�b�N�X���w�肵�A�X�e�[�W�J�ڃt���O�𗧂Ă�B
	/// </summary>
	/// <param name="i">���̃X�e�[�W�̃C���f�b�N�X�����߂�</param>
	/// <param name="fade_delta">nullptr�̏ꍇ�A���̃X�e�[�W�̃C���f�b�N�X�͌��݂̃C���f�b�N�X�{�P</param>
	void NextStage(uint32_t i, float fade_delta = 2.0f);
	
	/// <summary>
	/// �X�e�[�W�G�f�B�^�̊J�n����
	/// </summary>
	void StartEditor();

	/// <summary>
	/// �X�e�[�W�G�f�B�^�̏I������
	/// </summary>
	void EndEditor();

	// ���݂̃X�e�[�W�̃A�N�^�Ƃ��ēo�^����B
	// �X�e�[�W���ړ������Ƃ��ɂ��̃A�N�^�͔j�������B
	// �G�����������Ƃ��ȂǂɎg���B
	void RegisterActor(const std::shared_ptr<Actor>& actor);

	void ActivateWarp();

private:

	void ChangeTutorialStage();

	void ChangeBossStage();

	void ChangeCommonStage(
		uint32_t stage_width,
		uint32_t stage_height,
		uint32_t enemy_appearance_amount,
		uint32_t item_appearance_amount);

	/// <summary>
	/// �����̃C���f�b�N�X�̃X�e�[�W�̃L���b�V�������݂���ꍇ�A�����ǂݍ��ݒn�`�𐶐�����B
	/// </summary>
	/// <param name="stage_index"></param>
	/// <returns>�L���b�V�������݂��Ȃ��ꍇ�@nullptr�@��Ԃ��B</returns>
	bool ImportStage(uint32_t stage_index);

	/// <summary>
	/// �ʏ�X�e�[�W�̃X�e�[�W�����f�[�^�̃L���b�V����j������B
	/// </summary>
	void RemoveStageCacheFile();

	/// <summary>
	/// ���C��AABB�ɂ��Փ˂��s��
	/// </summary>
	/// <param name="rs">������s���c�̃Z���̎n�_</param>
	/// <param name="re">�I�_</param>
	/// <param name="cs">������s�����̃Z���̎n�_</param>
	/// <param name="ce">�I�_</param>
	/// <param name="map">�n�`�}�b�v</param>
	/// <param name="ray_s"></param>
	/// <param name="ray_d"></param>
	/// <param name="ray_l"></param>
	/// <param name="out_norm"></param>
	/// <param name="out_near_len"></param>
	/// <returns>������s���Փ˂�����true</returns>
	bool CollisionTerrainByAABB(
		uint32_t rs, uint32_t re,
		uint32_t cs, uint32_t ce,
		const std::vector<std::vector<TerrainTile>>& map,
		const Vector3& ray_s,
		const Vector3& ray_d, float ray_l,
		Vector3& out_norm, float& out_near_len);

	/// <summary>
	/// ���f���ɑ΂���X�t�B�A�L���X�g�ɂ��Փ˂��s��
	/// </summary>
	/// <param name="rs">������s���c�̃Z���̎n�_</param>
	/// <param name="re">�I�_</param>
	/// <param name="cs">������s�����̃Z���̎n�_</param>
	/// <param name="ce">�I�_</param>
	/// <param name="terrain"></param>
	/// <param name="ray_s"></param>
	/// <param name="ray_e"></param>
	/// <param name="sphere_radius">�L���X�g���鋅�̑傫��</param>
	/// <param name="out_intersect_pos"></param>
	/// <param name="out_wall_norm"></param>
	/// <returns></returns>
	bool CollisionTerrainBySphereCast(
		uint32_t rs, uint32_t re,
		uint32_t cs, uint32_t ce,
		const Terrain& terrain,
		const Vector3& ray_s,
		const Vector3& ray_e,
		float sphere_radius,
		Vector3& out_intersect_pos,
		Vector3& out_wall_norm);

	/// <summary>
	/// ���̃X�e�[�W�̃A�N�^�Ƃ��Đ������ꂽ�I�u�W�F�N�g��
	/// �A�N�^�Ǘ��N���X���珜�O����B
	/// </summary>
	void RemoveCurrentStageActors();

private:

	std::optional<uint32_t> next_stage_index;
	uint32_t current_stage_index;
	Stage stage;
	TerrainRenderer terrain_renderer;
	Shader terrain_shader;

#ifdef _DEBUG
	StageEditor editor;
#endif // _DEBUG

};
