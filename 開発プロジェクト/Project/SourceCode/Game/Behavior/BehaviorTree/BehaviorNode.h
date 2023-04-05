#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "BehaviorAction.h"
#include "BehaviorData.h"
#include "BehaviorJudgement.h"
#include "BehaviorTree.h"

/* --- 宣言 ---- */

/// <summary>
/// ノード
/// </summary>
class BehaviorNode : public std::enable_shared_from_this<BehaviorNode>
{
public:

	/// <summary>
	/// ノードの作成
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="name">ノード名</param>
	/// <param name="parent_node">親ノード</param>
	/// <param name="sibling">兄弟ノード</param>
	/// <param name="priority">ノード選択の優先順位</param>
	/// <param name="select_rule">子クラス遷移時の選択方法</param>
	/// <param name="judgement">このノードに現在遷移できるかを判定するクラス</param>
	/// <param name="action">実行内容</param>
	/// <param name="hierarchy_num">階層番号</param>
	void Create(
		const std::shared_ptr<Actor>& actor,
		const char* name, 
		const std::shared_ptr<BehaviorNode>& parent_node, 
		const std::shared_ptr<BehaviorNode>& sibling, 
		uint32_t priority,
		BehaviorTree::SelectRule select_rule, 
		std::unique_ptr<BehaviorJudgement>&& judgement, 
		std::unique_ptr<BehaviorAction>&& action, 
		uint32_t hierarchy_num);

	/// <summary>
	/// ノードの検索
	/// </summary>
	/// <returns>見つからなかった場合はnullptrを返す</returns>
	std::shared_ptr<BehaviorNode> SearchNode(const char* node_name);

	/// <summary>
	/// 現在遷移することが可能な子ノードの中から指定の選択方法での推論を行う
	/// </summary>
	/// <param name="actor">選択ルールによっては必要になるデータ</param>
	/// <param name="data">選択ルールによっては必要になるデータ</param>
	/// <returns>推論によって選択された子ノード</returns>
	std::shared_ptr<BehaviorNode> Inference(const std::shared_ptr<Actor>& actor, BehaviorData* data);

	/// <summary>
	/// このノードへの遷移が行えるか判定を行う。
	/// </summary>
	/// <param name="actor"></param>
	/// <returns>０：遷移不可 １：遷移可 ー１：判定クラスなし</returns>
	int Judgement(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <returns>実行結果を返す</returns>
	BehaviorAction::State Run(float elapsed_time, const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// ノード名の取得
	/// </summary>
	const char* GetName() const { return name.c_str(); }

	/// <summary>
	/// ノード名のハッシュ値による比較
	/// </summary>
	bool CompareName(const char* dst);

	std::shared_ptr<BehaviorNode> GetParentNode() const { return w_parent_node.lock(); }

	/// <summary>
	/// 配列先頭の子ノード。なければnullptr
	/// </summary>
	std::shared_ptr<BehaviorNode> GetChildFirstNode() const { return (!child_nodes.empty()) ? child_nodes.at(0) : nullptr; }

	/// <summary>
	/// 配列末尾の子ノード。なければnullptr
	/// </summary>
	std::shared_ptr<BehaviorNode> GetChildLastNode() const { return (!child_nodes.empty()) ? child_nodes.at(child_nodes.size() - 1ULL) : nullptr; }

	std::shared_ptr<BehaviorNode> GetSiblingNode() const { return w_sibling_node.lock(); }

	void SetParentNode(std::shared_ptr<BehaviorNode> node) { this->w_parent_node = node; }
	void AddChildNode(const std::shared_ptr<BehaviorNode>& node) { child_nodes.emplace_back(node); }
	void SetSibling(const std::shared_ptr<BehaviorNode>& node) { this->w_sibling_node = node; }

	/// <summary>
	/// 階層番号の取得
	/// </summary>
	uint32_t GetHierarchyNum() const { return hierarchy_num; }

	/// <summary>
	/// ノード選択の優先順位の取得
	/// </summary>
	uint32_t GetPriority() const { return priority; }

	/// <summary>
	/// 実行データをもっているか
	/// </summary>
	/// <returns></returns>
	bool HasAction() { return action ? true : false; }

private:

	/// <summary>
	/// 最も優先度の高い（Priorityが最も０に近い）ノードを選択
	/// </summary>
	std::shared_ptr<BehaviorNode> SelectPriority(std::vector<std::shared_ptr<BehaviorNode>>& nodes);
	/// <summary>
	/// ランダムノード選択
	/// </summary>
	std::shared_ptr<BehaviorNode> SelectRandom(std::vector<std::shared_ptr<BehaviorNode>>& nodes);
	/// <summary>
	/// シーケンス、シーケンシャルルーピングノード選択
	/// </summary>
	/// <returns>実行可能なノードがない場合は nullptr を返す</returns>
	std::shared_ptr<BehaviorNode> SelectSequence(std::vector<std::shared_ptr<BehaviorNode>>& nodes, BehaviorData* data);

private:

	std::vector<std::shared_ptr<BehaviorNode>> child_nodes; // 子ノード

	std::string name;								// ノード名
	std::weak_ptr<BehaviorNode> w_parent_node;		// 親ノードの弱参照
	std::weak_ptr<BehaviorNode> w_sibling_node;		// 兄弟ノードの弱参照
	uint32_t priority;								// ノード選択の優先順位(値が０に近いほど優先順位が高い)
	BehaviorTree::SelectRule select_rule;			// 子クラス遷移時の選択方法
	std::unique_ptr<BehaviorJudgement> judgement;	// このノードに現在遷移できるかを判定するクラス
	std::unique_ptr<BehaviorAction> action;			// 実行内容
	uint32_t hierarchy_num;							// 階層番号

};
