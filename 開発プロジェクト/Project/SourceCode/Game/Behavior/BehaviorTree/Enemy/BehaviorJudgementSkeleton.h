#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorJudgementEnemy.h"

/* --- 宣言 ---- */

class JudgementBattleSkeleton : public BehaviorJudgement
{
public:
	JudgementBattleSkeleton() = default;
	~JudgementBattleSkeleton() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};