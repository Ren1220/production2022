#include "BehaviorSpider.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorActionSpider.h"
#include "BehaviorJudgementSpider.h"


/* --- ��` ---- */

BehaviorSpider::BehaviorSpider()
	: BehaviorEnemy()
{
	search_range = 7.5f;

	attack_point_range = 0.25f; // ���̓G�͉���������U���s�����s��
	attack_distance = 5.0f;

	pursuit_speed = 1.0f;
	pursuit_speed_max = 3.0f;

	wander_goal_point = {}; // �p�j�X�e�[�g���̖ڕW�n�_
	wanger_range = 5.0f; // �p�j���̖ڕW�n�_�͈̔�
	wander_speed = 0.25f;
	wander_speed_max = 0.5f;
}

BehaviorSpider::~BehaviorSpider()
{
}
 
void BehaviorSpider::Start()
{
	BehaviorEnemy::Start();

	AddNode(nullptr, "Root", 0, BehaviorTree::Priority, nullptr, nullptr);

	AddNode("Root",         "Damaged",      0, BehaviorTree::None, std::make_unique<JudgementDamagedEnemy>(), std::make_unique<ActionDamagedSpider>());
    AddNode("Root",			"Dead",         1, BehaviorTree::None, std::make_unique<JudgementDeadEnemy>(), std::make_unique<ActionDeadSpider>());
	AddNode("Root",         "Battle",       2, BehaviorTree::SequentialLooping, std::make_unique<JudgementButtleSpider>(), nullptr);
	AddNode("Battle",       "Pursuit",      0, BehaviorTree::None, nullptr, std::make_unique<ActionPursuitSpider>());
	AddNode("Battle",       "Facing",       1, BehaviorTree::None, nullptr, std::make_unique<ActionFaceSpider>());
	AddNode("Battle",       "AttackSelect", 2, BehaviorTree::Random, nullptr, nullptr);
	AddNode("AttackSelect", "Melee",        0, BehaviorTree::Sequence, nullptr, nullptr);
	AddNode("Melee",        "Approach",     0, BehaviorTree::None, nullptr, std::make_unique<ActionApproachSpider>());
	AddNode("Melee",        "Attack",       1, BehaviorTree::None, nullptr, std::make_unique<ActionAttackSpider>());
	AddNode("AttackSelect", "Launch",       1, BehaviorTree::None, nullptr, std::make_unique<ActionLaunchSpider>());

	AddNode("Root", "Wander", 3, BehaviorTree::SelectRule::None, nullptr, std::make_unique<ActionWanderSpider>());
}

void BehaviorSpider::Update(float elapsed_time)
{
	BehaviorEnemy::Update(elapsed_time);
}

void BehaviorSpider::ImGuiRender()
{
	BehaviorEnemy::ImGuiRender();
}
