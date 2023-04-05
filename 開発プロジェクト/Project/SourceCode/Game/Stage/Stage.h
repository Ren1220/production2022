#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>

#include "Terrain/Terrain.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class Stage
{
public:

	Stage() = default;
	virtual ~Stage() = default;

	virtual void Initialize();

	virtual void CreateTerrain(
		uint32_t stage_width, 
		uint32_t stage_height,
		uint32_t enemy_appearance_amount,
		uint32_t item_appearance_amount);

	virtual void CreateTerrain(const char* filepath);

	/// <summary>
	/// ���̃X�e�[�W�ɓ��ꂵ���ۂɍs������
	/// </summary>
	virtual void Enter(bool activate_warp = true);

	/// <summary>
	/// ���̃X�e�[�W��ޏꂵ���ۂɍs������
	/// </summary>
	virtual void Exit();

	void ExportStageData(uint32_t index);
	
	void ImGuiRender();

	std::pair<uint32_t, uint32_t> GetStageSize() const { return { terrain.GetMapWidth(), terrain.GetMapHeight() }; }
	const Terrain& GetTerrain() const { return terrain; }
	Terrain& GetTerrain() { return terrain; }

	const std::vector<std::shared_ptr<Actor>>& GetStageActors() const { return stage_actors;  
	}

	
	/// <summary>
	/// �����̃A�N�^�����̃t���A�̃A�N�^�Ƃ��Ĕz��ɓo�^����B
	/// �X�e�[�W��؂�ւ���ۂɁA�o�^�����A�N�^���폜����̂Ɏg���B
	/// </summary>
	void RegisterActor(const std::shared_ptr<Actor>& actor);

	void ActivateWarp();

	bool CanNext() const { return can_next; }

protected:

	uint32_t enemy_appearance_amount;
	uint32_t item_appearance_amount;

	// ���̃X�e�[�W�Ő��������A�N�^
	std::vector<std::shared_ptr<Actor>> stage_actors;

	// �n�`���
	Terrain terrain;

	EffekseerHandle warp_effect;

	bool can_next;
};
