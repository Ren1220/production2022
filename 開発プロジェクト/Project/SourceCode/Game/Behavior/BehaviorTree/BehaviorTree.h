#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Behavior.h"

/* --- 前方宣言 ---- */

class BehaviorAction;
class BehaviorData;
class BehaviorJudgement;
class BehaviorNode;

/* --- 宣言 ---- */

class BehaviorTree : public Behavior
{
public:

	BehaviorTree();
	virtual ~BehaviorTree() override;

	virtual void Start();
	virtual void Update(float elapsed_time);
	virtual void End();

	virtual void ImGuiRender();

	/// <summary>
	/// 子ノード遷移時の選択方法
	/// </summary>
	enum SelectRule
	{
		None,				// 無い末端ノード用
		Priority,			// 優先順位
		Sequence,			// シーケンス
		SequentialLooping,	// シーケンシャルルーピング
		Random,				// ランダム
	};

protected:

	/// <summary>
	/// ノード追加
	/// </summary>
	/// <param name="parent_name">親ノード名を指定。自分がどのノードに紐づくか、また兄弟ノードの探索に必要</param>
	/// <param name="entry_name">自分のノード名。実行ノードの検索などに使用</param>	
	/// <param name="priority">優先順位。ルールがプライオリティなどの場合に使用</param>
	/// <param name="select_rule">各中間ノードのルールを指定</param>
	/// <param name="judgement">ノードが実行できるか判断するオブジェクト</param>
	/// <param name="action">実際の行動処理のオブジェクト</param>
	void AddNode(
		const char* parent_name, 
		const char* entry_name, 
		uint32_t priority, 
		SelectRule select_rule, 
		std::unique_ptr<BehaviorJudgement>&& judgement, 
		std::unique_ptr<BehaviorAction>&& action);

	std::shared_ptr<BehaviorNode> Run(
		float elapsed_time,
		const std::shared_ptr<BehaviorNode>& action_node,
		BehaviorData* data) const;

private:

	/// <summary>
	/// 実行ノードを推論する
	/// </summary>
	std::shared_ptr<BehaviorNode> ActiveNodeInference(BehaviorData* data) const;

	/// <summary>
	/// シーケンスノードから推論開始
	/// </summary>
	/// <param name="sequence_node"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	std::shared_ptr<BehaviorNode> SequenceBack(
		const std::shared_ptr<BehaviorNode>& sequence_node, 
		BehaviorData* data) const;

private:

	void ClearAllNodes(std::shared_ptr<BehaviorNode>& node);

	std::shared_ptr<BehaviorNode> root_node;	// ルートノード

	std::unique_ptr<BehaviorData> behavior_data;
	std::shared_ptr<BehaviorNode> active_node;
};
