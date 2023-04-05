#include "BehaviorJudgementBoss.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"

#include "BehaviorBoss.h"

/* --- 定義 ---- */

bool JudgementBattlegBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    return actor->GetBehavior<BehaviorBoss>()->IsFindPlayer();
}

bool JudgementBattleRoaringBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    return actor->GetBehavior<BehaviorBoss>()->IsModeChanged();
}

bool JudgementDesperateBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    return actor->GetBehavior<BehaviorBoss>()->GetMode() == BehaviorBoss::Desperate;
}

bool JudgementEnragedBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    return actor->GetBehavior<BehaviorBoss>()->GetMode() == BehaviorBoss::Enraged;
}

bool JudgementBackBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
    return (player_pos - actor->GetWorldPosition()).Length() < actor->GetBehavior<BehaviorBoss>()->GetTransitionBackRange();
}

bool JudgementReturnBoss::Judgement(const std::shared_ptr<Actor>& actor)
{
    return false;
    return actor->GetBehavior<BehaviorBoss>()->CalcDistanceToInitPos() > 10.0f;
}
