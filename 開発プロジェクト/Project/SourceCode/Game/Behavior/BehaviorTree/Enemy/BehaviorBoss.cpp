#include "BehaviorBoss.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Component/Health/Health.h"

#include "BehaviorJudgementBoss.h"
#include "BehaviorActionBoss.h"

/* --- 定義 ---- */

BehaviorBoss::BehaviorBoss()
	: BehaviorEnemy(), mode(Usual), is_mode_changed(false)
{
	is_find_player = false;
	search_range = 7.5f;
	attack_range = 3.0f;
	transition_back_range = 4.0f;
}

BehaviorBoss::~BehaviorBoss()
{
}

void BehaviorBoss::Start()
{
	BehaviorEnemy::Start();

	// 初期位置を確保しておく
	init_pos = GetActor()->GetWorldPosition();

	AddNode(nullptr, "Root", 0, BehaviorTree::Priority, nullptr, nullptr);

	AddNode("Root", "Dead", 0, BehaviorTree::None, std::make_unique<JudgementDeadEnemy>(), std::make_unique<ActionDeadBoss>());
	AddNode("Root", "Damaged", 1, BehaviorTree::None, std::make_unique<JudgementDamagedEnemy>(), std::make_unique<ActionDamagedBoss>());
	AddNode("Root", "Battle", 2, BehaviorTree::Priority, std::make_unique<JudgementBattlegBoss>(), nullptr);
	AddNode("Battle", "B_Roaring", 0, BehaviorTree::None, std::make_unique<JudgementBattleRoaringBoss>(), std::make_unique<ActionRoaringBattleBoss>());

	AddNode("Battle", "B_Return", 1, BehaviorTree::None, std::make_unique<JudgementReturnBoss>(), std::make_unique<ActionReturnBoss>());

	AddNode("Battle", "Desparate", 2, BehaviorTree::Random, std::make_unique<JudgementDesperateBoss>(), nullptr);
	AddNode("Desparate", "D_Melee", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("D_Melee", "D_M_Pursuit", 0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitBoss>());
	AddNode("D_Melee", "D_M_Tackle", 1, BehaviorTree::None, nullptr, std::make_unique<ActionTackleBoss>());
	AddNode("Desparate", "D_Ranged", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("D_Ranged", "D_R_Face0", 0, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("D_Ranged", "D_R_Back",  1, BehaviorTree::None, nullptr, std::make_unique<ActionBackBoss>());
	AddNode("D_Ranged", "D_R_Face1", 2, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("D_Ranged", "D_R_Launch", 3, BehaviorTree::None, nullptr, std::make_unique<ActionLaunchBoss>());
	AddNode("Desparate", "D_JumpATK", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("D_JumpATK", "D_J_Back", 0, BehaviorTree::None, nullptr, std::make_unique<ActionBackBoss>());
	AddNode("D_JumpATK", "D_J_Face", 1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("D_JumpATK", "D_J_JumpATK", 2, BehaviorTree::None, nullptr, std::make_unique<ActionJumpATKBoss>());

	AddNode("Battle", "Enraged", 3, BehaviorTree::Random, std::make_unique<JudgementEnragedBoss>(), nullptr);
	AddNode("Enraged", "E_Melee", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("E_Melee", "E_M_Pursuit", 0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitBoss>());
	AddNode("E_Melee", "E_M_Face", 1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("E_Melee", "E_M_ATK1", 2, BehaviorTree::None, nullptr, std::make_unique<ActionAttack1Boss>());
	AddNode("Enraged", "E_Ranged", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("E_Ranged", "E_R_Face0", 0, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("E_Ranged", "E_R_Back",  1, BehaviorTree::None, nullptr, std::make_unique<ActionBackBoss>());
	AddNode("E_Ranged", "E_R_Face1", 2, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("E_Ranged", "E_R_Launch", 3, BehaviorTree::None, nullptr, std::make_unique<ActionLaunchBoss>());

	AddNode("Battle", "Usual", 4, BehaviorTree::Random, nullptr, nullptr);
	AddNode("Usual", "U_Melee", 0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("U_Melee", "U_M_Pursuit", 0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitBoss>());
	AddNode("U_Melee", "U_M_Face", 1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("U_Melee", "U_M_ATK1", 2, BehaviorTree::None, nullptr, std::make_unique<ActionAttack1Boss>());

	AddNode("Root", "Idle", 3, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("Idle", "I_Wait", 0, BehaviorTree::None, nullptr, std::make_unique<ActionIdleBoss>());
	AddNode("Idle", "I_Face", 1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceBoss>());
	AddNode("Idle", "I_Roaring", 2, BehaviorTree::None, nullptr, std::make_unique<ActionRoaringBoss>());
}

void BehaviorBoss::Update(float elapsed_time)
{
	if (auto health = GetActor()->GetComponent<Health>())
	{
		float ratio{ static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth()) };

		// 通常 -> 怒り
		if (mode == Mode::Usual)
		{
			if (ratio < 0.8f)
			{
				is_mode_changed = true;
				mode = Mode::Enraged;
			}
		}
		// 怒り -> 本気
		else if (mode == Mode::Enraged)
		{
			if (ratio < 0.5f)
			{
				is_mode_changed = true;
				mode = Mode::Desperate;
			}
		}
	}

	BehaviorEnemy::Update(elapsed_time);
}

void BehaviorBoss::ImGuiRender()
{
}

float BehaviorBoss::CalcDistanceToInitPos() const
{
	const Vector3& pos{ GetActor()->GetWorldPosition() };
	return init_pos.Distance(pos);
}
