#include "BehaviorTree.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "BehaviorAction.h"
#include "BehaviorData.h"
#include "BehaviorJudgement.h"
#include "BehaviorNode.h"

/* --- 定義 ---- */

BehaviorTree::BehaviorTree()
	: Behavior()
{
}

BehaviorTree::~BehaviorTree()
{
}

void BehaviorTree::Start()
{
	behavior_data = std::make_unique<BehaviorData>();
}

void BehaviorTree::Update(float elapsed_time)
{
	// 現在実行されているノードが無い
	if (active_node == nullptr)
	{
		// 次に実行するノードを推論する
		active_node = ActiveNodeInference(behavior_data.get());
	}
	// 現在実行するノードがある
	else
	{
		// ビヘイビアツリーからノードを実行
		active_node = Run(elapsed_time, active_node, behavior_data.get());
	}
}

void BehaviorTree::End()
{
}

void BehaviorTree::ImGuiRender()
{
	const char* node_name{ "None" };
	if (active_node)
	{
		node_name = active_node->GetName();
	}

	ImGui::Text(node_name);
}

std::shared_ptr<BehaviorNode> BehaviorTree::ActiveNodeInference(BehaviorData* data) const
{
	// データを初期化してから推論を行う
	data->Initialize();
	return root_node->Inference(GetActor(), data);
}

std::shared_ptr<BehaviorNode> BehaviorTree::SequenceBack(
	const std::shared_ptr<BehaviorNode>& sequence_node, 
	BehaviorData* data) const
{
	// シーケンスノードから推論を行う
	return sequence_node->Inference(GetActor(), data);
}

void BehaviorTree::AddNode(
	const char* parent_name,
	const char* entry_name, 
	uint32_t priority, 
	SelectRule select_rule, 
	std::unique_ptr<BehaviorJudgement>&& judgement,
	std::unique_ptr<BehaviorAction>&& action)
{
	if (parent_name)
	{
		// 引数の名前のノードを検索する
		std::shared_ptr<BehaviorNode> parent_node{ root_node->SearchNode(parent_name) };

		if (parent_node)
		{
			std::shared_ptr<BehaviorNode> sibling{ parent_node->GetChildLastNode() };

			// 新しいノードを作成する
			auto add_node{ std::make_shared<BehaviorNode>() };
			add_node->Create(
				GetActor(),
				entry_name, 
				parent_node, 
				sibling, 
				priority, 
				select_rule,
				std::move(judgement), 
				std::move(action), 
				parent_node->GetHierarchyNum() + 1U);

			// 作成したノードを引数名の親ノードの子ノードとして登録する
			parent_node->AddChildNode(add_node);

			return;
		}

		_ASSERT_EXPR(parent_node, L"引数の名前の親ノードが存在しません");
	}
	// 親の名前が空の場合
	else
	{
		_ASSERT_EXPR(root_node == nullptr, L"既にルートノードが存在しています");

		// ルートノードがまだ作成されていない
		if (root_node == nullptr)
		{
			// ルートノードの作成する
			root_node = std::make_shared<BehaviorNode>();
			root_node->Create(
				GetActor(),
				entry_name, 
				nullptr, 
				nullptr, 
				priority, 
				select_rule, 
				std::move(judgement),
				std::move(action), 
				1U);

			return;
		}
	}
}

std::shared_ptr<BehaviorNode> BehaviorTree::Run(
	float elapsed_time, 
	const std::shared_ptr<BehaviorNode>& action_node, 
	BehaviorData* data) const
{
	// ノードを実行する
	BehaviorAction::State state{ action_node->Run(elapsed_time, GetActor()) };

	// 実行が成功したか
	switch (state)
	{
	case BehaviorAction::State::Run:
		return action_node;
	case BehaviorAction::State::Failed:
		return nullptr;
	case BehaviorAction::State::Complete:
	{
		// シーケンスの途中か判断する
		std::shared_ptr<BehaviorNode> sequence_node{ data->PopSequenceNode() };

		// 途中でないなら終了
		if (sequence_node == nullptr)
		{
			return nullptr;
		}
		else
		{
			// 途中ならそこから始める
			return SequenceBack(sequence_node, data);
		}
	}
	} 

	return action_node;
}

void BehaviorTree::ClearAllNodes(std::shared_ptr<BehaviorNode>& node)
{
	node.reset();
}
