#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "BehaviorAction.h"
#include "BehaviorData.h"
#include "BehaviorJudgement.h"
#include "BehaviorTree.h"

/* --- �錾 ---- */

/// <summary>
/// �m�[�h
/// </summary>
class BehaviorNode : public std::enable_shared_from_this<BehaviorNode>
{
public:

	/// <summary>
	/// �m�[�h�̍쐬
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="name">�m�[�h��</param>
	/// <param name="parent_node">�e�m�[�h</param>
	/// <param name="sibling">�Z��m�[�h</param>
	/// <param name="priority">�m�[�h�I���̗D�揇��</param>
	/// <param name="select_rule">�q�N���X�J�ڎ��̑I����@</param>
	/// <param name="judgement">���̃m�[�h�Ɍ��ݑJ�ڂł��邩�𔻒肷��N���X</param>
	/// <param name="action">���s���e</param>
	/// <param name="hierarchy_num">�K�w�ԍ�</param>
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
	/// �m�[�h�̌���
	/// </summary>
	/// <returns>������Ȃ������ꍇ��nullptr��Ԃ�</returns>
	std::shared_ptr<BehaviorNode> SearchNode(const char* node_name);

	/// <summary>
	/// ���ݑJ�ڂ��邱�Ƃ��\�Ȏq�m�[�h�̒�����w��̑I����@�ł̐��_���s��
	/// </summary>
	/// <param name="actor">�I�����[���ɂ���Ă͕K�v�ɂȂ�f�[�^</param>
	/// <param name="data">�I�����[���ɂ���Ă͕K�v�ɂȂ�f�[�^</param>
	/// <returns>���_�ɂ���đI�����ꂽ�q�m�[�h</returns>
	std::shared_ptr<BehaviorNode> Inference(const std::shared_ptr<Actor>& actor, BehaviorData* data);

	/// <summary>
	/// ���̃m�[�h�ւ̑J�ڂ��s���邩������s���B
	/// </summary>
	/// <param name="actor"></param>
	/// <returns>�O�F�J�ڕs�� �P�F�J�ډ� �[�P�F����N���X�Ȃ�</returns>
	int Judgement(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// ���s����
	/// </summary>
	/// <returns>���s���ʂ�Ԃ�</returns>
	BehaviorAction::State Run(float elapsed_time, const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// �m�[�h���̎擾
	/// </summary>
	const char* GetName() const { return name.c_str(); }

	/// <summary>
	/// �m�[�h���̃n�b�V���l�ɂ���r
	/// </summary>
	bool CompareName(const char* dst);

	std::shared_ptr<BehaviorNode> GetParentNode() const { return w_parent_node.lock(); }

	/// <summary>
	/// �z��擪�̎q�m�[�h�B�Ȃ����nullptr
	/// </summary>
	std::shared_ptr<BehaviorNode> GetChildFirstNode() const { return (!child_nodes.empty()) ? child_nodes.at(0) : nullptr; }

	/// <summary>
	/// �z�񖖔��̎q�m�[�h�B�Ȃ����nullptr
	/// </summary>
	std::shared_ptr<BehaviorNode> GetChildLastNode() const { return (!child_nodes.empty()) ? child_nodes.at(child_nodes.size() - 1ULL) : nullptr; }

	std::shared_ptr<BehaviorNode> GetSiblingNode() const { return w_sibling_node.lock(); }

	void SetParentNode(std::shared_ptr<BehaviorNode> node) { this->w_parent_node = node; }
	void AddChildNode(const std::shared_ptr<BehaviorNode>& node) { child_nodes.emplace_back(node); }
	void SetSibling(const std::shared_ptr<BehaviorNode>& node) { this->w_sibling_node = node; }

	/// <summary>
	/// �K�w�ԍ��̎擾
	/// </summary>
	uint32_t GetHierarchyNum() const { return hierarchy_num; }

	/// <summary>
	/// �m�[�h�I���̗D�揇�ʂ̎擾
	/// </summary>
	uint32_t GetPriority() const { return priority; }

	/// <summary>
	/// ���s�f�[�^�������Ă��邩
	/// </summary>
	/// <returns></returns>
	bool HasAction() { return action ? true : false; }

private:

	/// <summary>
	/// �ł��D��x�̍����iPriority���ł��O�ɋ߂��j�m�[�h��I��
	/// </summary>
	std::shared_ptr<BehaviorNode> SelectPriority(std::vector<std::shared_ptr<BehaviorNode>>& nodes);
	/// <summary>
	/// �����_���m�[�h�I��
	/// </summary>
	std::shared_ptr<BehaviorNode> SelectRandom(std::vector<std::shared_ptr<BehaviorNode>>& nodes);
	/// <summary>
	/// �V�[�P���X�A�V�[�P���V�������[�s���O�m�[�h�I��
	/// </summary>
	/// <returns>���s�\�ȃm�[�h���Ȃ��ꍇ�� nullptr ��Ԃ�</returns>
	std::shared_ptr<BehaviorNode> SelectSequence(std::vector<std::shared_ptr<BehaviorNode>>& nodes, BehaviorData* data);

private:

	std::vector<std::shared_ptr<BehaviorNode>> child_nodes; // �q�m�[�h

	std::string name;								// �m�[�h��
	std::weak_ptr<BehaviorNode> w_parent_node;		// �e�m�[�h�̎�Q��
	std::weak_ptr<BehaviorNode> w_sibling_node;		// �Z��m�[�h�̎�Q��
	uint32_t priority;								// �m�[�h�I���̗D�揇��(�l���O�ɋ߂��قǗD�揇�ʂ�����)
	BehaviorTree::SelectRule select_rule;			// �q�N���X�J�ڎ��̑I����@
	std::unique_ptr<BehaviorJudgement> judgement;	// ���̃m�[�h�Ɍ��ݑJ�ڂł��邩�𔻒肷��N���X
	std::unique_ptr<BehaviorAction> action;			// ���s���e
	uint32_t hierarchy_num;							// �K�w�ԍ�

};
