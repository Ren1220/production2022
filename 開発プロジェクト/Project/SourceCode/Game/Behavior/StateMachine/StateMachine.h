#pragma once

/* --- �T�v ---- */



/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <unordered_map>
#include <optional>

#include "Game/Behavior/Behavior.h"

/* --- �O���錾 ---- */

class StateMachineState;
class StateMachineHierarchicalState;

/* --- �錾 ---- */

class StateMachine : public Behavior
{
public:

	StateMachine(uint32_t state_key);
	StateMachine(uint32_t state_key, uint32_t sub_state_key);

	virtual ~StateMachine() override;

	void Start()override;
	void Update(float elapsed_time) override;
	void End() override;
	void ImGuiRender() override;

private:

	void UpdateState();
	void TransitionStateAndSubState(uint32_t state_key, uint32_t sub_state_key);

public:

	/// <summary>
	/// �J�ڑO�̃X�e�[�g�����݂���ꍇ�͂��̏I������
	/// �X�e�[�g��ύX��							 
	/// �J�ڌ�̃X�e�[�g�̊J�n�������s��			 
	/// �A�j���[�V�������ݒ肳��Ă���ꍇ�͂��̍Đ����s��
	/// </summary>
	void TransitionState(uint32_t key);

	/// <summary>
	/// <para>�J�ڑO�T�u�X�e�[�g�����݂���ꍇ�͂��̏I������</para>
	/// <para>�T�u�X�e�[�g��ύX��							  </para>
	/// <para>�J�ڌ�̃T�u�X�e�[�g�̊J�n�������s��			  </para>
	/// <para>�A�j���[�V�������ݒ肳��Ă���ꍇ�͂��̍Đ����s��</para>
	/// </summary>
	void TransitionSubState(uint32_t key);

	/// <summary>
	/// <para>�q�G�����L�[�X�e�[�g��o�^����</para>
	/// <para>�����ɁA���̃X�e�[�g�ɑJ�ڂ���Ƃ��ɍĐ�����A�j���[�V������ݒ肷��</para>
	/// <para>�Ȃ��A�����̏��ԏ�T�u�X�e�[�g�̃A�j���[�V�����ݒ肪�D�悳���</para>
	/// </summary>
	void RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state, uint32_t animation_key, bool animation_loop_flag);
	void RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state);

	/// <summary>
	/// �T�u�X�e�[�g��o�^����
	/// �����ɁA���̃X�e�[�g�ɑJ�ڂ���Ƃ��ɍĐ�����A�j���[�V������ݒ肷��
	/// �Ȃ��A�����̏��ԏ�T�u�X�e�[�g�̃A�j���[�V�����ݒ肪�D�悳���
	/// </summary>
	void RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state, uint32_t animation_key, bool animation_loop_flag);
	void RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state);

	/// <summary>
	/// ���̍X�V���Ɉ����̃X�e�[�g�ɑJ�ڂ���
	/// </summary>
	void SetNextState(uint32_t key);

	/// <summary>
	/// ���̍X�V���Ɉ����̃T�u�X�e�[�g�ɑJ�ڂ���
	/// </summary>
	void SetNextSubState(uint32_t key);

	uint32_t FindCurrentStateKey() const;
	std::shared_ptr<StateMachineState> GetCurrentState();
	const std::shared_ptr<StateMachineHierarchicalState>& GetStateOfKey(uint32_t key) const;

	uint32_t GetCurrentStateKey() const { return current_state_key; };
	const std::optional<uint32_t>& GetCurrentSubStateKey() const { return current_sub_state_key; };

	/// <summary>
	/// �X�e�[�g�J�ډ\��
	/// </summary>
	bool CanTransition() const { return can_transition; }
	void SetTransitionFlag(bool b) { can_transition = b; }

protected:

	uint32_t current_state_key;						// ���݂̃X�e�[�g�̃L�[
	uint32_t next_state_key;						// ����  �X�e�[�g�̃L�[
	std::optional<uint32_t> current_sub_state_key;	// ���݂̃T�u�X�e�[�g�̃L�[
	std::optional<uint32_t> next_sub_state_key;		// ����  �T�u�X�e�[�g�̃L�[

	std::shared_ptr<StateMachineHierarchicalState> current_state;       // ���݂̃X�e�[�g
	std::unordered_map<uint32_t, std::shared_ptr<StateMachineHierarchicalState>> registered_states; // �o�^�����X�e�[�g

	bool can_transition; // �X�e�[�g�J�ډ\

};
