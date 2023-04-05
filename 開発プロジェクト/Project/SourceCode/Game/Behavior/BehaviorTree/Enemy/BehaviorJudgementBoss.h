#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorJudgementEnemy.h"

/* --- 宣言 ---- */

class JudgementBattlegBoss : public BehaviorJudgement
{
public:
	JudgementBattlegBoss() = default;
	~JudgementBattlegBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementBattleRoaringBoss : public BehaviorJudgement
{
public:
	JudgementBattleRoaringBoss() = default;
	~JudgementBattleRoaringBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementDesperateBoss : public BehaviorJudgement
{
public:
	JudgementDesperateBoss() = default;
	~JudgementDesperateBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementEnragedBoss : public BehaviorJudgement
{
public:
	JudgementEnragedBoss() = default;
	~JudgementEnragedBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementBackBoss : public BehaviorJudgement
{
public:
	JudgementBackBoss() = default;
	~JudgementBackBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementReturnBoss : public BehaviorJudgement
{
public:
	JudgementReturnBoss() = default;
	~JudgementReturnBoss() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};
