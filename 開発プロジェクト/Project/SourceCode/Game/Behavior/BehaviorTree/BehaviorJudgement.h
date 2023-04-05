#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <string>

#include "../../Actor/Actor.h"

/* --- 宣言 ---- */

class BehaviorJudgement
{
public:

	BehaviorJudgement() = default;
	virtual ~BehaviorJudgement() = default;

	virtual void Initialize(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// 次のノードに遷移が可能な状態か判定する。
	/// </summary>
	/// <returns></returns>
	virtual bool Judgement(const std::shared_ptr<Actor>& actor) = 0;

};
