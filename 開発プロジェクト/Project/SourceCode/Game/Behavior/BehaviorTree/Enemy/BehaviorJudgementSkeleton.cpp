#include "BehaviorJudgementSkeleton.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorSkeleton.h"

/* --- 定義 ---- */

bool JudgementBattleSkeleton::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

	return behavior->GetPlayerVector().LengthSquare() < behavior->GetSearchRange() * behavior->GetSearchRange();
}
