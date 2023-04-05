#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorEnemy.h"

/* --- �錾 ---- */

class BehaviorSpider : public BehaviorEnemy
{
public:

	BehaviorSpider();
	~BehaviorSpider() override;

	void Start() override;
	void Update(float elapsed_time) override;

	void ImGuiRender() override;

	float GetAttackPointRange() const { return attack_point_range; }
	float GetAttackDistance() const { return attack_distance; }

	float GetSearchRange() const { return search_range; }

	float GetPursuitSpeed() const { return pursuit_speed; }
	float GetPursuitSpeedMax() const { return pursuit_speed_max; }

	const Vector3& GetWanderGoalPoint() const { return wander_goal_point; }
	void SetWanderGoalPoint(const Vector3& p) { wander_goal_point = p; }

	float GetWanderRange() const { return wanger_range; }
	float GetWanderSpeed() const { return wander_speed; }
	float GetWanderSpeedMax() const { return wander_speed_max; }

private:

	float search_range;

	float attack_point_range; // ���̓G�͉���������U���s�����s��
	float attack_distance;

	float pursuit_speed;
	float pursuit_speed_max;

	Vector3 wander_goal_point; // �p�j�X�e�[�g���̖ڕW�n�_
	float wanger_range; // �p�j���̖ڕW�n�_�͈̔�
	float wander_speed;
	float wander_speed_max;

};

