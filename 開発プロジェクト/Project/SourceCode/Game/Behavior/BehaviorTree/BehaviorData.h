#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>
#include <stack>
#include <map>

#include "../../Actor/Actor.h"

#include "BehaviorTree.h"
#include "BehaviorNode.h"


/* --- �錾 ---- */

class BehaviorData
{
public:

	BehaviorData();
	~BehaviorData();

	/// <summary>
	/// �V�[�P���X�m�[�h�̃v�b�V��
	/// </summary>
	void PushSequenceNode(std::shared_ptr<BehaviorNode> node) { w_sequence_stack.emplace(node); }

	/// <summary>
	/// �V�[�P���X�m�[�h�̃|�b�v
	/// </summary>
	/// <returns> ��Ȃ� nullptr ��Ԃ� </returns>
	std::shared_ptr<BehaviorNode> PopSequenceNode();

	/// <summary>
	/// �V�[�P���X�X�e�b�v�̎擾
	/// </summary>
	int32_t GetSequenceStep(const char* name);

	/// <summary>
	/// �V�[�P���X�X�e�b�v�̐ݒ�
	/// </summary>
	void SetSequenceStep(const char* name, int32_t step);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

private:

	std::stack<std::weak_ptr<BehaviorNode>> w_sequence_stack;	// ���s���钆�ԃm�[�h�X�^�b�N
	std::map<std::string, int32_t> run_sequence_step_map;			// ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
};
