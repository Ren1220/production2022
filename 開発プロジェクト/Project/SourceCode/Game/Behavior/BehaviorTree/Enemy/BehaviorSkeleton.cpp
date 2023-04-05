#include "BehaviorSkeleton.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorActionSkeleton.h"
#include "BehaviorJudgementSkeleton.h"

/* --- 定義 ---- */

BehaviorSkeleton::BehaviorSkeleton()
	: BehaviorEnemy()
{
	search_range      = 5.0f;
	pursuit_range     = 7.5f;
	pursuit_speed     = 1.0f;
	pursuit_speed_max = 2.0f;

	wander_goal_point = {}; // 徘徊ステート時の目標地点
	wanger_range      = 5.0f; // 徘徊時の目標地点の範囲
	wander_speed      = 0.25f;
	wander_speed_max  = 0.5f;

	attack_range      = 1.5f;
}

BehaviorSkeleton ::~BehaviorSkeleton()
{
}

void BehaviorSkeleton::Start()
{
	BehaviorEnemy::Start();

	AddNode(nullptr, "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

	AddNode("Root", "Dead",   0, BehaviorTree::None, std::make_unique<JudgementDeadEnemy>(), std::make_unique<ActionDeadSkeleton>());
	AddNode("Root", "Damage", 1, BehaviorTree::None, std::make_unique<JudgementDamagedEnemy>(), std::make_unique<ActionDamagedSkeleton>());

	AddNode("Root",  "Battle",   2, BehaviorTree::Sequence, std::make_unique<JudgementBattleSkeleton>(), nullptr);
	AddNode("Battle", "Melee",   0, BehaviorTree::SequentialLooping, std::make_unique<JudgementBattleSkeleton>(), nullptr);
	AddNode("Melee",  "Pursuit", 0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitSkeleton>());
	AddNode("Melee",  "Facing",  1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceSkeleton>());
	AddNode("Melee",  "Attack",  2, BehaviorTree::None, nullptr, std::make_unique<ActionAttackSkeleton>());

	AddNode("Root", "Wander", 3, BehaviorTree::SelectRule::None, nullptr, std::make_unique<ActionWanderSkeleton>());

}

void BehaviorSkeleton::Update(float elapsed_time)
{
	BehaviorEnemy::Update(elapsed_time);
}

void BehaviorSkeleton::ImGuiRender()
{
	BehaviorEnemy::ImGuiRender();
}
