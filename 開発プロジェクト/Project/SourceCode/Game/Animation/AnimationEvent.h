#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <functional>
#include <memory>
#include <vector>

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

/// <summary>
/// �A�j���[�V�����C�x���g
/// <para>Animator�R���|�[�l���g�ł��̃C���X�^���X��o�^���A���s�\�ȃR�[���o�b�N�֐������s����</para>
/// </summary>
class AnimationCallbackEvent 
{
public:

	AnimationCallbackEvent(const std::function<void(const std::shared_ptr<Actor>&)>& event_function);
	virtual ~AnimationCallbackEvent() = default;

	/// <summary>
	/// ���[�v���ɍs������������
	/// </summary>
	virtual void InitializePerLoop() {};

	/// <summary>
	/// �C�x���g�����s�\��
	/// </summary>
	virtual bool CanExecuteEvent(float time) const { return false; }

	/// <summary>
	/// �C�x���g�̎��s
	/// </summary>
	virtual void Execute(const std::shared_ptr<Actor>& actor);

protected:

	std::function<void(const std::shared_ptr<Actor>&)> event_function;
};

/// <summary>
/// ���[�v���Ɏw�莞�Ԉȍ~��x�������s�����C�x���g
/// </summary>
class AnimationEventOncePerLoop : public AnimationCallbackEvent
{
public:
	/// <summary></summary>
	/// <param name="event_function"></param>
	/// <param name="evente_begin_time">���̎��Ԉȍ~�ň�x�C�x���g�����s����</param>
	AnimationEventOncePerLoop(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float evente_begin_time);

	/// <summary>
	/// ���[�v���ɍs������������
	/// </summary>
	void InitializePerLoop();

	/// <summary>
	/// �C�x���g�����s�\��
	/// </summary>
	bool CanExecuteEvent(float time) const;

	/// <summary>
	/// �C�x���g�̎��s
	/// </summary>
	void Execute(const std::shared_ptr<Actor>& actor);

private:

	bool has_executed_once_in_this_loop;
	float evente_begin_time;
};

/// <summary>
/// �w�莞�Ԕ͈͓��Ŗ��t���[�����s�����C�x���g
/// </summary>
class AnimationEventPerFrame : public AnimationCallbackEvent
{
public:
	/// <summary></summary>
	/// <param name="event_function"></param>
	/// <param name="event_begin_time">�C�x���g���s�\�͈�</param>
	/// <param name="event_end_time">�C�x���g���s�\�͈�</param>
	AnimationEventPerFrame(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float event_begin_time,
		float event_end_time);

	/// <summary>
	/// �C�x���g�����s�\��
	/// </summary>
	bool CanExecuteEvent(float time) const;

private:

	std::pair<float, float> event_execute_time_range;
};
