#include "BehaviorJudgementEnemy.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"

#include "BehaviorEnemy.h"

/* --- 定義 ---- */

bool JudgementDamagedEnemy::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorEnemy>() };
	return behavior->IsDamaged();
}

bool JudgementDeadEnemy::Judgement(const std::shared_ptr<Actor>& actor)
{
	return actor->GetComponent<Health>()->IsDead();
}
