#include "BehaviorNode.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Utility/RandomFunction.h"

#include "BehaviorTree.h"

/* --- 定義 ---- */

void BehaviorNode::Create(
	const std::shared_ptr<Actor>& actor,
	const char* name, 
	const std::shared_ptr<BehaviorNode>& parent_node, 
	const std::shared_ptr<BehaviorNode>& sibling, 
	uint32_t priority, 
	BehaviorTree::SelectRule select_rule,
	std::unique_ptr<BehaviorJudgement>&& judgement, 
	std::unique_ptr<BehaviorAction>&& action, 
	uint32_t hierarchy_num)
{
	this->name = name;
	this->w_parent_node = parent_node;
	this->w_sibling_node = sibling;
	this->priority = priority;
	this->select_rule = select_rule;
	this->judgement = std::move(judgement); // Action, Judgementはここで初期化
	if (this->judgement) this->judgement->Initialize(actor);
	this->action = std::move(action);
	if (this->action) this->action->Initialize(actor); 
	this->hierarchy_num = hierarchy_num;
}

std::shared_ptr<BehaviorNode> BehaviorNode::SearchNode(const char* node_name)
{
	// 自身の名前と一致した場合は自身のポインタを返す
	if (CompareName(node_name))
	{
		return shared_from_this();
	}

	// 一致しなかった場合は子ノードの検索を行う
	for (const auto& cn : child_nodes)
	{
		if (auto found_node = cn->SearchNode(node_name))
		{
			return found_node;
		}
	}

	return nullptr;
}

std::shared_ptr<BehaviorNode> BehaviorNode::Inference(
	const std::shared_ptr<Actor>& actor,
	BehaviorData* data)
{
	// 選択可能ノード
	std::vector<std::shared_ptr<BehaviorNode>> selectables{};

	for (const auto& cn : child_nodes)
	{
		switch (cn->Judgement(actor))
		{
		case 1:  // 判定を行いtrueの場合
		case -1: // 判定クラスがない場合
			selectables.emplace_back(cn);
			break;
		case 0: // 判定を行いfalseの場合
		default:
			break;
		}
	}

	// 選択したノード
	std::shared_ptr<BehaviorNode> select_node{};

	// 設定したルールに従ってノードを決定する
	switch (select_rule)
	{
	case BehaviorTree::SelectRule::None:
		// 選択ルールが存在しないということは選択肢は一つのみなので、配列の頭のノードを格納する
		select_node = selectables.front();
		break;
	case BehaviorTree::SelectRule::Priority:
		// 優先順位
		select_node = SelectPriority(selectables);
		break;
	case BehaviorTree::SelectRule::Sequence:
	case BehaviorTree::SelectRule::SequentialLooping:
		// シーケンス
		select_node = SelectSequence(selectables, data);
		break;
	case BehaviorTree::SelectRule::Random:
		// ランダムで決定
		select_node = SelectRandom(selectables);
		break;
	}

	if (select_node)
	{
		// 決定したノードに実行クラスがあればreturnする
		if (select_node->HasAction())
		{
			select_node->action->Begin(actor);
			return select_node;
		}
		else
		{
			// 実行クラスがなければ、そのノードの推論を行う
			select_node = select_node->Inference(actor, data);
		}
	}

	return select_node;
}

int BehaviorNode::Judgement(const std::shared_ptr<Actor>& actor)
{
	// 実行可能判定オブジェクトが存在する場合実行し、その結果を返す
	if (judgement)
	{
		return judgement->Judgement(actor) ? 1 : 0;
	}

	return -1;
}

BehaviorAction::State BehaviorNode::Run(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	// 実行オブジェクトが存在する場合実行し、その結果を返す
	if (action)
	{
		return action->Run(elapsed_time, actor);
	}

	// 存在しない場合は実行失敗
	return BehaviorAction::State::Failed;
}

bool BehaviorNode::CompareName(const char* dst)
{
	return std::hash<std::string>()(name) == std::hash<std::string>()(dst);
}

std::shared_ptr<BehaviorNode> BehaviorNode::SelectPriority(std::vector<std::shared_ptr<BehaviorNode>>& nodes)
{
	std::shared_ptr<BehaviorNode> select;
	uint32_t priority{ UINT32_MAX };

	// 優先度が最も高い(値が最も０に近い)ノードを探す
	for (const auto& node : nodes)
	{
		if (node->priority < priority)
		{
			// select_node を更新する
			select = node;
			// priority の更新
			priority = node->priority;
		}
	}

	// 優先度が最も高いノードを返す
	return select;
}

std::shared_ptr<BehaviorNode> BehaviorNode::SelectRandom(std::vector<std::shared_ptr<BehaviorNode>>& nodes)
{
	// ノード配列のサイズで乱数を取得して値を格納する
	size_t select_num{ Random::GetValue(0ULL, nodes.size() - 1ULL) };

	// ランダムで取得した配列番号のノードを返す
	return nodes.at(select_num);
}

std::shared_ptr<BehaviorNode> BehaviorNode::SelectSequence(
	std::vector<std::shared_ptr<BehaviorNode>>& nodes,
	BehaviorData* data)
{
	// 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
	int32_t step{ data->GetSequenceStep(name.c_str()) };

	// 中間ノードに登録されているノード数以上の大きさの場合
	if (step >= child_nodes.size())
	{
		switch (select_rule)
		{
		case BehaviorTree::SelectRule::Sequence:
			// ルールが Sequence のときは次に実行できるノードがないため、nullptrをリターン
			return nullptr;
		case BehaviorTree::SelectRule::SequentialLooping:
			// ルールが SequentialLooping のときは再び最初から実行するため、stepに0を代入
			step = 0;
			break;
		default:
			assert(!"error");
			break;
		}
	}

	// 実行可能リストに登録されているノードの数ループを行う
	for (const auto& node : nodes)
	{
		if (child_nodes.at(step)->CompareName(node->GetName()))
		{
			// 実行中の中間ノードをプッシュする
			data->PushSequenceNode(shared_from_this());
			// 実行中の中間ノードと次のステップ数を設定する
			data->SetSequenceStep(name.c_str(), step + 1);
			// ステップ番号の子ノードを実行ノードとして返す
			return child_nodes.at(step);
		}
	}

	// 指定された中間ノードに実行可能なノードがないので nullptr を返す
	return nullptr;
}
