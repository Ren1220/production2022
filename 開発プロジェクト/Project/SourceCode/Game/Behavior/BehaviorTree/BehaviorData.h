#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>
#include <stack>
#include <map>

#include "../../Actor/Actor.h"

#include "BehaviorTree.h"
#include "BehaviorNode.h"


/* --- 宣言 ---- */

class BehaviorData
{
public:

	BehaviorData();
	~BehaviorData();

	/// <summary>
	/// シーケンスノードのプッシュ
	/// </summary>
	void PushSequenceNode(std::shared_ptr<BehaviorNode> node) { w_sequence_stack.emplace(node); }

	/// <summary>
	/// シーケンスノードのポップ
	/// </summary>
	/// <returns> 空なら nullptr を返す </returns>
	std::shared_ptr<BehaviorNode> PopSequenceNode();

	/// <summary>
	/// シーケンスステップの取得
	/// </summary>
	int32_t GetSequenceStep(const char* name);

	/// <summary>
	/// シーケンスステップの設定
	/// </summary>
	void SetSequenceStep(const char* name, int32_t step);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

private:

	std::stack<std::weak_ptr<BehaviorNode>> w_sequence_stack;	// 実行する中間ノードスタック
	std::map<std::string, int32_t> run_sequence_step_map;			// 実行中の中間ノードのステップを記録
};
