#include "BehaviorTree.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorAction.h"
#include "BehaviorData.h"
#include "BehaviorJudgement.h"
#include "BehaviorNode.h"

/* --- ��` ---- */

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
	// ���ݎ��s����Ă���m�[�h������
	if (active_node == nullptr)
	{
		// ���Ɏ��s����m�[�h�𐄘_����
		active_node = ActiveNodeInference(behavior_data.get());
	}
	// ���ݎ��s����m�[�h������
	else
	{
		// �r�w�C�r�A�c���[����m�[�h�����s
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
	// �f�[�^�����������Ă��琄�_���s��
	data->Initialize();
	return root_node->Inference(GetActor(), data);
}

std::shared_ptr<BehaviorNode> BehaviorTree::SequenceBack(
	const std::shared_ptr<BehaviorNode>& sequence_node, 
	BehaviorData* data) const
{
	// �V�[�P���X�m�[�h���琄�_���s��
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
		// �����̖��O�̃m�[�h����������
		std::shared_ptr<BehaviorNode> parent_node{ root_node->SearchNode(parent_name) };

		if (parent_node)
		{
			std::shared_ptr<BehaviorNode> sibling{ parent_node->GetChildLastNode() };

			// �V�����m�[�h���쐬����
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

			// �쐬�����m�[�h���������̐e�m�[�h�̎q�m�[�h�Ƃ��ēo�^����
			parent_node->AddChildNode(add_node);

			return;
		}

		_ASSERT_EXPR(parent_node, L"�����̖��O�̐e�m�[�h�����݂��܂���");
	}
	// �e�̖��O����̏ꍇ
	else
	{
		_ASSERT_EXPR(root_node == nullptr, L"���Ƀ��[�g�m�[�h�����݂��Ă��܂�");

		// ���[�g�m�[�h���܂��쐬����Ă��Ȃ�
		if (root_node == nullptr)
		{
			// ���[�g�m�[�h�̍쐬����
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
	// �m�[�h�����s����
	BehaviorAction::State state{ action_node->Run(elapsed_time, GetActor()) };

	// ���s������������
	switch (state)
	{
	case BehaviorAction::State::Run:
		return action_node;
	case BehaviorAction::State::Failed:
		return nullptr;
	case BehaviorAction::State::Complete:
	{
		// �V�[�P���X�̓r�������f����
		std::shared_ptr<BehaviorNode> sequence_node{ data->PopSequenceNode() };

		// �r���łȂ��Ȃ�I��
		if (sequence_node == nullptr)
		{
			return nullptr;
		}
		else
		{
			// �r���Ȃ炻������n�߂�
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
