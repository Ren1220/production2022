#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Arithmetic/Arithmetic.h"

#include "../../Actor/Actor.h"

/* --- 宣言 ---- */

/// <summary>
/// 行動処理基底クラス
/// </summary>
class BehaviorAction
{
public:

	/// <summary>
	/// 実行情報
	/// </summary>
	enum class State
	{
		Run,		// 実行中。次のフレームにおいでもこのノードの処理を行う。
		Failed,		// 実行失敗。ルートノードから推論を再開する
		Complete	// 実行成功。次のノードに遷移
	};

	BehaviorAction() = default;
	virtual ~BehaviorAction() = default;

	virtual void Initialize(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// このノードに遷移したときに実行する関数
	/// </summary>
	/// <param name="actor"></param>
	virtual void Begin(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// 実行処理。返す実行情報によってツリーの行動が変化する。
	/// <para>Run      ：実行中。次のフレームにおいてもこのノードの処理を行う。</para>
	/// <para>Failed   ：実行失敗。ルートノードから推論を再開する</para>
	/// <para>Complete ：実行成功。次のノードに遷移</para>
	/// </summary>
	virtual State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) = 0;
};
