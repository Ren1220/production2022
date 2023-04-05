#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorJudgement.h"

/* --- 宣言 ---- */

/// <summary>
/// ダメージ判定はBehaviorクラス内のフラグを用いる。
/// ダメージを受けた時にノードが中断され、次にInferenceが実行された際にはHealthコンポーネントが更新されているため
/// </summary>
class JudgementDamagedEnemy : public BehaviorJudgement
{
public:
	JudgementDamagedEnemy() = default;
	~JudgementDamagedEnemy() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementDeadEnemy : public BehaviorJudgement
{
public:
	JudgementDeadEnemy() = default;
	~JudgementDeadEnemy() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};
