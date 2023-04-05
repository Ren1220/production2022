#include "BehaviorJudgementEnemy.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"

#include "BehaviorEnemy.h"

/* --- ��` ---- */

bool JudgementDamagedEnemy::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorEnemy>() };
	return behavior->IsDamaged();
}

bool JudgementDeadEnemy::Judgement(const std::shared_ptr<Actor>& actor)
{
	return actor->GetComponent<Health>()->IsDead();
}
