#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <unordered_map>
#include <optional>

/* --- �O���錾 ---- */

class Actor;
class StateMachine;

/* --- �錾 ---- */

class StateMachineState
{
public:
	StateMachineState(const std::shared_ptr<Actor>& actor);
	virtual ~StateMachineState();

	/// <summary>
	/// �X�e�[�g�ɓ��������̊֐�
	/// </summary>
	virtual void Start() = 0;
	/// <summary>
	/// �X�e�[�g�Ŏ��s����֐�
	/// </summary>
	virtual void Execute(float elapsed_time) = 0;
	/// <summary>
	/// �X�e�[�g����o�Ă����Ƃ��̊֐�
	/// </summary>
	virtual void End() = 0;
public:

	std::shared_ptr<StateMachine> GetStateMachine() const;
	std::shared_ptr<Actor> GetActor() const;

	void StopAnimation() const;

protected:

	std::weak_ptr<Actor> w_actor; //  ���̃X�e�[�g�}�V�������A�N�^

	int32_t step; // �X�e�b�v
};

class StateMachineHierarchicalState : public StateMachineState
{
public:
	StateMachineHierarchicalState(const std::shared_ptr<Actor>& actor);
	~StateMachineHierarchicalState() override;

	void Start() override = 0;
	void Execute(float elapsed_time) override = 0;
	void End() override = 0;
	/// <summary>
	/// <para>�J�ڑO�̃T�u�X�e�[�g�����݂���ꍇ�͂��̏I������</para>
	/// <para>�T�u�X�e�[�g��ύX��							  </para>
	/// <para>�J�ڌ�̃T�u�X�e�[�g�̊J�n�������s��			  </para>
	/// <para>�A�j���[�V�������ݒ肳��Ă���ꍇ�͂��̍Đ����s��</para>
	/// </summary>
	void TransitionSubState(uint32_t key);

	void RegisterSubState(uint32_t key, std::shared_ptr<StateMachineState> state);

	std::shared_ptr<StateMachineState> GetCurrentSubState();

	/// <summary>
	/// �X�e�[�g�}�V���̍X�V�����Ŏg��
	/// �X�e�[�g�̑J�ڂɂ͎g��Ȃ�
	/// </summary>
	void SetCurrentSubState(uint32_t key);

	const std::shared_ptr<StateMachineState>& GetSubStateOfKey(uint32_t key) const;

	/// <summary>
	/// �T�u�X�e�[�g�̃C���f�b�N�X�̎擾
	/// </summary>
	/// <returns>������Ȃ��ꍇnullopt��Ԃ�</returns>
	std::optional<uint32_t> FindCurrentSubStateKey();

protected:

	/// <summary>
	/// �T�u�X�e�[�g�̎��s����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void ExecuteSubState(float elapsed_time) const;

protected:

	std::unordered_map<uint32_t, std::shared_ptr<StateMachineState>> sub_states; // �K�w�X�e�[�g�̎��T�u�X�e�[�g�̃o�b�t�@
	std::shared_ptr<StateMachineState> current_sub_state;               // ���ݎ��s����Ă���T�u�X�e�[�g
};
