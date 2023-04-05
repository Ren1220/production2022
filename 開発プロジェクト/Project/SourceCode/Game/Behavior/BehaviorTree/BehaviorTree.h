#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Behavior.h"

/* --- �O���錾 ---- */

class BehaviorAction;
class BehaviorData;
class BehaviorJudgement;
class BehaviorNode;

/* --- �錾 ---- */

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
	/// �q�m�[�h�J�ڎ��̑I����@
	/// </summary>
	enum SelectRule
	{
		None,				// �������[�m�[�h�p
		Priority,			// �D�揇��
		Sequence,			// �V�[�P���X
		SequentialLooping,	// �V�[�P���V�������[�s���O
		Random,				// �����_��
	};

protected:

	/// <summary>
	/// �m�[�h�ǉ�
	/// </summary>
	/// <param name="parent_name">�e�m�[�h�����w��B�������ǂ̃m�[�h�ɕR�Â����A�܂��Z��m�[�h�̒T���ɕK�v</param>
	/// <param name="entry_name">�����̃m�[�h���B���s�m�[�h�̌����ȂǂɎg�p</param>	
	/// <param name="priority">�D�揇�ʁB���[�����v���C�I���e�B�Ȃǂ̏ꍇ�Ɏg�p</param>
	/// <param name="select_rule">�e���ԃm�[�h�̃��[�����w��</param>
	/// <param name="judgement">�m�[�h�����s�ł��邩���f����I�u�W�F�N�g</param>
	/// <param name="action">���ۂ̍s�������̃I�u�W�F�N�g</param>
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
	/// ���s�m�[�h�𐄘_����
	/// </summary>
	std::shared_ptr<BehaviorNode> ActiveNodeInference(BehaviorData* data) const;

	/// <summary>
	/// �V�[�P���X�m�[�h���琄�_�J�n
	/// </summary>
	/// <param name="sequence_node"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	std::shared_ptr<BehaviorNode> SequenceBack(
		const std::shared_ptr<BehaviorNode>& sequence_node, 
		BehaviorData* data) const;

private:

	void ClearAllNodes(std::shared_ptr<BehaviorNode>& node);

	std::shared_ptr<BehaviorNode> root_node;	// ���[�g�m�[�h

	std::unique_ptr<BehaviorData> behavior_data;
	std::shared_ptr<BehaviorNode> active_node;
};
