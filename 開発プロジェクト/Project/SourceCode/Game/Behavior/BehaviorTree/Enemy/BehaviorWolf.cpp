#include "BehaviorWolf.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorActionWolf.h"
#include "BehaviorJudgementWolf.h"

/* --- 定義 ---- */

BehaviorWolf::BehaviorWolf(bool has_summoned)
	: BehaviorEnemy(), has_summoned(has_summoned)
{
	search_range = 5.0f;
	pursuit_range = 7.5f;
	pursuit_speed = 1.0f;
	pursuit_speed_max = 3.0f;

	wander_goal_point = {}; // 徘徊ステート時の目標地点
	wanger_range = 5.0f; // 徘徊時の目標地点の範囲
	wander_speed = 0.5f;
	wander_speed_max = 1.0f;

	attack_range = 1.0f;
	attack_cooltime = 1.1f;
	cooltimer = 0.0f;
}

BehaviorWolf::~BehaviorWolf()
{
}

void BehaviorWolf::Start()
{
	BehaviorEnemy::Start();

	AddNode(nullptr, "Root", 0, BehaviorTree::Priority, nullptr, nullptr);

	AddNode("Root",    "Summoned", 0, BehaviorTree::None, std::make_unique<JudgementHasSummonedWolf>(), std::make_unique<ActionAppearanceWolf>());
	AddNode("Root",    "Dead",     1, BehaviorTree::None, std::make_unique<JudgementDeadEnemy>(), std::make_unique<ActionDeadWolf>());
	AddNode("Root",    "Damaged",  2, BehaviorTree::None, std::make_unique<JudgementDamagedEnemy>(), std::make_unique<ActionDamagedWolf>());
	AddNode("Root",    "Battle",   3, BehaviorTree::Priority, std::make_unique<JudgementBattleWolf>(), nullptr);
	AddNode("Battle",  "B_Summon",   0, BehaviorTree::Sequence, std::make_unique<JudgementCanSummonWolf>(), nullptr);
	AddNode("B_Summon",  "B_S_Facing",   0, BehaviorTree::None, nullptr, std::make_unique<ActionFaceSummonWolf>());
	AddNode("B_Summon",  "B_S_Action",   1, BehaviorTree::None, nullptr, std::make_unique<ActionSummonWolf>());
	AddNode("Battle",  "B_Melee",    1, BehaviorTree::SequentialLooping, nullptr, nullptr);
	AddNode("B_Melee",   "B_M_Pursuit",  0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitWolf>());
	AddNode("B_Melee",   "B_M_Facing",   1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceWolf>());
	AddNode("B_Melee",   "B_M_Attack",   2, BehaviorTree::None, nullptr, std::make_unique<ActionAttackWolf>());

	AddNode("Root", "Wander", 4, BehaviorTree::SelectRule::None, nullptr, std::make_unique<ActionWanderWolf>());
}

void BehaviorWolf::Update(float elapsed_time)
{
	// 攻撃のクールタイムに使う
	cooltimer -= elapsed_time;

	BehaviorEnemy::Update(elapsed_time);
}

void BehaviorWolf::ImGuiRender()
{
	BehaviorEnemy::ImGuiRender();
}
