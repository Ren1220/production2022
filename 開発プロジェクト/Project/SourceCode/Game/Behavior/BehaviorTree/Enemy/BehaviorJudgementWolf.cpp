#include "BehaviorJudgementWolf.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorWolf.h"

/* --- 定義 ---- */

bool JudgementHasSummonedWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	// 実行するのは一度だけ
	if (has_called) return false;

	has_called = true;
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	// 召喚された場合のみ
	if (behavior->HasSummoned()) return true;

	return false;
}

bool JudgementCanSummonWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	// 実行するのは一度だけ
	if (has_called) return false;

	has_called = true;
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	// 召喚されていない場合のみ
	if (!behavior->HasSummoned()) return true;

	return false;
}

bool JudgementBattleWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	return behavior->GetPlayerVector().LengthSquare() < behavior->GetSearchRange() * behavior->GetSearchRange();
}
