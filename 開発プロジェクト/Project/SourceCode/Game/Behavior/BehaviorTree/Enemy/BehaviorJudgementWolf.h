#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorJudgementEnemy.h"

/* --- 宣言 ---- */


class JudgementBattleWolf : public BehaviorJudgement
{
public:

	JudgementBattleWolf() = default;
	~JudgementBattleWolf() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementHasSummonedWolf : public BehaviorJudgement
{
public:

	JudgementHasSummonedWolf() : has_called(false) {};
	~JudgementHasSummonedWolf() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;

private:

	bool has_called;
};

class JudgementCanSummonWolf : public BehaviorJudgement
{
public:

	JudgementCanSummonWolf() : has_called(false) {};
	~JudgementCanSummonWolf() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;

private:

	bool has_called;
};



