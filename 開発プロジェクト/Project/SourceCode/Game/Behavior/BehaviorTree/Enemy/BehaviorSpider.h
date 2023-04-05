#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorEnemy.h"

/* --- 宣言 ---- */

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

	float attack_point_range; // この敵は遠距離から攻撃行動を行う
	float attack_distance;

	float pursuit_speed;
	float pursuit_speed_max;

	Vector3 wander_goal_point; // 徘徊ステート時の目標地点
	float wanger_range; // 徘徊時の目標地点の範囲
	float wander_speed;
	float wander_speed_max;

};

