#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorJudgementEnemy.h"

/* --- 宣言 ---- */

class JudgementButtleSpider : public BehaviorJudgement
{
public:
	JudgementButtleSpider() = default;
	~JudgementButtleSpider() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

