#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorEnemy.h"

/* --- �錾 ---- */

class BehaviorBoss : public BehaviorEnemy
{
public:

	// �{�X�̏�� �̗͂�1/3 �؂邲�Ƃɕω�����
	enum Mode { Usual, Enraged, Desperate };

	BehaviorBoss();
	~BehaviorBoss() override;

	void Start() override;
	void Update(float elapsed_time) override;

	void ImGuiRender() override;

	Mode GetMode() const { return mode; }
	float GetTransitionBackRange() const { return transition_back_range; }

	bool IsModeChanged() const { return is_mode_changed; }
	void SetModeChanged(bool b) { is_mode_changed = b; }

	bool IsFindPlayer() const { return is_find_player; }
	void SetFindPlayer(bool b) { is_find_player = b; }
	
	float GetSearchRange() const { return search_range; }
	float GetAttackRange() const { return attack_range; }
	void SetAttackRange(float f) { attack_range = f; }

	float CalcDistanceToInitPos() const;
	const Vector3& GetInitPos() const { return init_pos; }

private:

	Mode mode;
	bool is_mode_changed; // ��Ԃ��ω������^�C�~���O

	float transition_back_range;

	float search_range;
	bool is_find_player;

	float attack_range;

	Vector3 init_pos; // �����ʒu

};

