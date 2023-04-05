#include "BehaviorNode.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/RandomFunction.h"

#include "BehaviorTree.h"

/* --- ��` ---- */

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
	this->judgement = std::move(judgement); // Action, Judgement�͂����ŏ�����
	if (this->judgement) this->judgement->Initialize(actor);
	this->action = std::move(action);
	if (this->action) this->action->Initialize(actor); 
	this->hierarchy_num = hierarchy_num;
}

std::shared_ptr<BehaviorNode> BehaviorNode::SearchNode(const char* node_name)
{
	// ���g�̖��O�ƈ�v�����ꍇ�͎��g�̃|�C���^��Ԃ�
	if (CompareName(node_name))
	{
		return shared_from_this();
	}

	// ��v���Ȃ������ꍇ�͎q�m�[�h�̌������s��
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
	// �I���\�m�[�h
	std::vector<std::shared_ptr<BehaviorNode>> selectables{};

	for (const auto& cn : child_nodes)
	{
		switch (cn->Judgement(actor))
		{
		case 1:  // ������s��true�̏ꍇ
		case -1: // ����N���X���Ȃ��ꍇ
			selectables.emplace_back(cn);
			break;
		case 0: // ������s��false�̏ꍇ
		default:
			break;
		}
	}

	// �I�������m�[�h
	std::shared_ptr<BehaviorNode> select_node{};

	// �ݒ肵�����[���ɏ]���ăm�[�h�����肷��
	switch (select_rule)
	{
	case BehaviorTree::SelectRule::None:
		// �I�����[�������݂��Ȃ��Ƃ������Ƃ͑I�����͈�݂̂Ȃ̂ŁA�z��̓��̃m�[�h���i�[����
		select_node = selectables.front();
		break;
	case BehaviorTree::SelectRule::Priority:
		// �D�揇��
		select_node = SelectPriority(selectables);
		break;
	case BehaviorTree::SelectRule::Sequence:
	case BehaviorTree::SelectRule::SequentialLooping:
		// �V�[�P���X
		select_node = SelectSequence(selectables, data);
		break;
	case BehaviorTree::SelectRule::Random:
		// �����_���Ō���
		select_node = SelectRandom(selectables);
		break;
	}

	if (select_node)
	{
		// ���肵���m�[�h�Ɏ��s�N���X�������return����
		if (select_node->HasAction())
		{
			select_node->action->Begin(actor);
			return select_node;
		}
		else
		{
			// ���s�N���X���Ȃ���΁A���̃m�[�h�̐��_���s��
			select_node = select_node->Inference(actor, data);
		}
	}

	return select_node;
}

int BehaviorNode::Judgement(const std::shared_ptr<Actor>& actor)
{
	// ���s�\����I�u�W�F�N�g�����݂���ꍇ���s���A���̌��ʂ�Ԃ�
	if (judgement)
	{
		return judgement->Judgement(actor) ? 1 : 0;
	}

	return -1;
}

BehaviorAction::State BehaviorNode::Run(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	// ���s�I�u�W�F�N�g�����݂���ꍇ���s���A���̌��ʂ�Ԃ�
	if (action)
	{
		return action->Run(elapsed_time, actor);
	}

	// ���݂��Ȃ��ꍇ�͎��s���s
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

	// �D��x���ł�����(�l���ł��O�ɋ߂�)�m�[�h��T��
	for (const auto& node : nodes)
	{
		if (node->priority < priority)
		{
			// select_node ���X�V����
			select = node;
			// priority �̍X�V
			priority = node->priority;
		}
	}

	// �D��x���ł������m�[�h��Ԃ�
	return select;
}

std::shared_ptr<BehaviorNode> BehaviorNode::SelectRandom(std::vector<std::shared_ptr<BehaviorNode>>& nodes)
{
	// �m�[�h�z��̃T�C�Y�ŗ������擾���Ēl���i�[����
	size_t select_num{ Random::GetValue(0ULL, nodes.size() - 1ULL) };

	// �����_���Ŏ擾�����z��ԍ��̃m�[�h��Ԃ�
	return nodes.at(select_num);
}

std::shared_ptr<BehaviorNode> BehaviorNode::SelectSequence(
	std::vector<std::shared_ptr<BehaviorNode>>& nodes,
	BehaviorData* data)
{
	// �w�肳��Ă��钆�ԃm�[�h�̃V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
	int32_t step{ data->GetSequenceStep(name.c_str()) };

	// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̑傫���̏ꍇ
	if (step >= child_nodes.size())
	{
		switch (select_rule)
		{
		case BehaviorTree::SelectRule::Sequence:
			// ���[���� Sequence �̂Ƃ��͎��Ɏ��s�ł���m�[�h���Ȃ����߁Anullptr�����^�[��
			return nullptr;
		case BehaviorTree::SelectRule::SequentialLooping:
			// ���[���� SequentialLooping �̂Ƃ��͍Ăэŏ�������s���邽�߁Astep��0����
			step = 0;
			break;
		default:
			assert(!"error");
			break;
		}
	}

	// ���s�\���X�g�ɓo�^����Ă���m�[�h�̐����[�v���s��
	for (const auto& node : nodes)
	{
		if (child_nodes.at(step)->CompareName(node->GetName()))
		{
			// ���s���̒��ԃm�[�h���v�b�V������
			data->PushSequenceNode(shared_from_this());
			// ���s���̒��ԃm�[�h�Ǝ��̃X�e�b�v����ݒ肷��
			data->SetSequenceStep(name.c_str(), step + 1);
			// �X�e�b�v�ԍ��̎q�m�[�h�����s�m�[�h�Ƃ��ĕԂ�
			return child_nodes.at(step);
		}
	}

	// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�ȃm�[�h���Ȃ��̂� nullptr ��Ԃ�
	return nullptr;
}
