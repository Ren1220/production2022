#include "BehaviorJudgementSpider.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorSpider.h"

/* --- 定義 ---- */

bool JudgementButtleSpider::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSpider>() };

	return behavior->GetPlayerVector().LengthSquare() < behavior->GetSearchRange() * behavior->GetSearchRange();
}
