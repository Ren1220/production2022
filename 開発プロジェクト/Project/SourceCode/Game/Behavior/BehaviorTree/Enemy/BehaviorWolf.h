#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorEnemy.h"

/* --- 宣言 ---- */

class BehaviorWolf : public BehaviorEnemy
{
public:

	BehaviorWolf(bool has_summoned);
	~BehaviorWolf() override;

	void Start() override;
	void Update(float elapsed_time) override;

	void ImGuiRender() override;

	bool HasSummoned() const { return has_summoned; }

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

	float GetAttackCoolTime() const { return attack_cooltime; }
	float GetCoolTimer() const { return cooltimer; }
	void SetCoolTime(float f) { cooltimer = f; }

private:

	bool has_summoned;

	float search_range;
	float pursuit_range;
	float pursuit_speed;
	float pursuit_speed_max;

	Vector3 wander_goal_point; // 徘徊ステート時の目標地点
	float wanger_range; // 徘徊時の目標地点の範囲
	float wander_speed;
	float wander_speed_max;


	float attack_range;
	float attack_cooltime; // 余りにも攻撃頻度が高いため、クールタイムを設けた
	float cooltimer;

};
