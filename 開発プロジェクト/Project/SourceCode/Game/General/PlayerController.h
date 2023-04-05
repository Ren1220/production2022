#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class PlayerController
{
public:

	PlayerController();
	~PlayerController();

	void Initialize();
	void Update(float elapsed_time);
	void EndProcess();

	/// <summary>
	/// �v���C���[�����S�����Ƃ��ɌĂ΂�鏈��
	/// </summary>
	void DeadPlayer();

	void ImGuiRender();

	void SetPlayer(const std::shared_ptr<Actor>& player);

	/// <summary>
	/// ActorManager�Ŗ��O�������\�����A�Q�Ƃ���񐔂������Ȃ肻���Ȃ̂ł�����̕������Ԃ�悢
	/// </summary>
	std::shared_ptr<Actor> GetPlayer() const;

	/// <summary>
	/// �^�[�Q�b�g�@�\��p���Č��݃^�[�Q�b�g���Ă���G
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Actor> GetTarget() const;

	/// <summary>
	/// �t�B�[���h��̃A�C�e�����E����͈�
	/// </summary>
	float GetItemPickupRange() const { return item_pickup_range; }

	std::shared_ptr<Actor> GetJumpAttackTarget() const;
	void SelectJumpAttackTarget(const std::vector<std::weak_ptr<Actor>>& w_attackers);
	void ClearJumpAttackTarget();

	bool CanTransitionJumpAttack() const { return can_transition_jump_attack; }
	void SetTransitionJumpAttack(bool b) { can_transition_jump_attack = b; }

private:

	/// <summary>
	/// (������)�G�̃^�[�Q�b�g
	/// </summary>
	void TargetEnemy();

	/// <summary>
	/// �v���C���[����̋����A���̑Ώۂɑ΂��Ă̌�����ɂ���ĕ]���l�����߁A
	/// �ł������Ώۂ�Ԃ��B
	/// </summary>
	std::shared_ptr<Actor> SearchTarget();

	std::weak_ptr<Actor> w_player;
	std::weak_ptr<Actor> w_target;
	std::weak_ptr<Actor> w_jump_attack_target;
	float item_pickup_range;

	bool can_transition_jump_attack;
};
