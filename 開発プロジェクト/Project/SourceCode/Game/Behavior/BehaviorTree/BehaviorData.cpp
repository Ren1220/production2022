#include "BehaviorData.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorNode.h"

/* --- ��` ---- */

BehaviorData::BehaviorData()
{
    Initialize();
}

BehaviorData::~BehaviorData()
{
}

std::shared_ptr<BehaviorNode> BehaviorData::PopSequenceNode()
{
    // �f�[�^�����݂��Ȃ��ꍇ
    if (w_sequence_stack.empty())
    {
        return nullptr;
    }

    std::shared_ptr<BehaviorNode> pop_node{ nullptr };

    // �f�[�^�����݂���ꍇ
    if (pop_node = w_sequence_stack.top().lock())
    {
        // ���o�����f�[�^���폜
        w_sequence_stack.pop();
    }

    return pop_node;
}

int32_t BehaviorData::GetSequenceStep(const char* name)
{
    // �����̃L�[�̃X�e�b�v��������Ȃ������ꍇ
    if (run_sequence_step_map.find(name) == run_sequence_step_map.end())
    {
        // �X�e�b�v�O�ŐV�����z��ɓo�^����
        run_sequence_step_map.emplace(std::make_pair(name, 0));
    }

    // �����̃L�[�̒l��Ԃ�
    return run_sequence_step_map.at(name);
}

void BehaviorData::SetSequenceStep(const char* name, int32_t step)
{
    run_sequence_step_map[name] = step;
}

void BehaviorData::Initialize()
{
    run_sequence_step_map.clear();

    w_sequence_stack = std::stack<std::weak_ptr<BehaviorNode>>();
    _ASSERT_EXPR(w_sequence_stack.empty(), L"�R���e�i������ɏ���������Ă��܂���");
}
