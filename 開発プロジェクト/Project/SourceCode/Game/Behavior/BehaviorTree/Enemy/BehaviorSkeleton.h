#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorEnemy.h"

/* --- �錾 ---- */

class BehaviorSkeleton : public BehaviorEnemy
{
public:

	BehaviorSkeleton();
	~BehaviorSkeleton() override;

	void Start() override;
	void Update(float elapsed_time) override;

	void ImGuiRender() override;

	float GetSearchRange() const { return search_range; }
	float GetPursuitRange() const { return pursuit_range; }
	float GetPursuitSpeed() const { return pursuit_speed; }
	float GetPursuitSpeedMax() const { return pursuit_speed_max; }

	float GetAttackRange()	const { return attack_range; }

	const Vector3& GetWanderGoalPoint() const { return wander_goal_point; }
	void SetWanderGoalPoint(const Vector3& p) { wander_goal_point = p; }

	float GetWanderRange() const { return wanger_range; }
	float GetWanderSpeed() const { return wander_speed; }
	float GetWanderSpeedMax() const { return wander_speed_max; }

private:

	float search_range;
	float pursuit_range;
	float pursuit_speed;
	float pursuit_speed_max;

	Vector3 wander_goal_point; // �p�j�X�e�[�g���̖ڕW�n�_
	float wanger_range; // �p�j���̖ڕW�n�_�͈̔�
	float wander_speed;
	float wander_speed_max;


	float attack_range;
};

