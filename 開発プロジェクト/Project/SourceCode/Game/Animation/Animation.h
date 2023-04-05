#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>
#include <string>

#include "Graphics/Model/GLTFModelData.h"

#include "AnimationEvent.h"

/* --- �O���錾 ---- */

class Actor;
class GLTFSkinnedMesh;

/* --- �錾 ---- */

class Animation
{
public:

	Animation(size_t animation_index);

	/// <summary>
	/// <para>���f���N���X�̃A�j���[�V�����m�[�h�ɃA�j���[�V�������ʂ�K�p����B</para>
	/// </summary>
	/// <param name="model"></param>
	/// <param name="time">�A�j���[�V�����̎��ԁB�A�j���[�V�������I�������炻�̏I�����Ԃ�ݒ肷��</param>
	/// <param name="has_played">�A�j���[�V�������I��������true��ݒ肷��</param>
	void AnimateModelNode(GLTFSkinnedMesh* model, float& time, bool& has_played);

	/// <summary>
	/// <para>���f���N���X�̃A�j���[�V�����m�[�h�ɃA�j���[�V�������ʂ�K�p����B</para>
	/// <para>���[�v�����Ƃ��e�A�j���[�V�����C�x���g�̃��[�v�����������������s�����B</para>
	/// </summary>
	/// <param name="model"></param>
	/// <param name="time">�A�j���[�V�����̎��ԁB�A�j���[�V���������[�v������A�I�����ԂŊ������]��̎��Ԃ�ݒ肷��</param>
	/// <param name="has_looped">�A�j���[�V���������[�v������true��ݒ肷��</param>
	void AnimateModelNodeLoop(GLTFSkinnedMesh* model, float& time, bool& has_looped);

	/// <summary>
	/// �w�莞�Ԕ͈͓��Ŗ��t���[�����s�����A�j���[�V�����C�x���g��o�^����
	/// </summary>
	/// <param name="event_function">���s����C�x���g</param>
	/// <param name="event_begin_time">�C�x���g�J�n����</param>
	/// <param name="event_end_time">�C�x���g�I������</param>
	void AddEventPerFrame(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float event_begin_time,
		float event_end_time);

	/// <summary>
	/// ���[�v���Ɏw�莞�Ԉȍ~��x�������s�����C�x���g
	/// </summary>
	/// <param name="event_function">���s����C�x���g</param>
	/// <param name="evente_begin_time">���̎��Ԉȍ~�ň�x�C�x���g�����s����</param>
	void AddEventOncePerLoop(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float evente_begin_time);

	/// <summary>
	/// ���̃A�j���V������ύX�����ۂɎ��s�����C�x���g
	/// </summary>
	void SetEventWhenExit(
		const std::function<void(const std::shared_ptr<Actor>&)>& exit_event_function);

	/// <summary>
	/// ���̃A�j���V�����ɕύX�����ۂɎ��s�����C�x���g
	/// </summary>
	void SetEventWhenEnter(
		const std::function<void(const std::shared_ptr<Actor>&)>& enter_event_function);

	/// <summary>
	/// �A�j���[�V�����̏I�����Ԃ��v������
	/// </summary>
	float MeasureAnimationTimeEnd(const GLTFSkinnedMesh* model) const;

	/// <summary>
	/// ���ݎ��s�\�ȃA�j���[�V�����C�x���g�����s����
	/// </summary>
	void ExecuteActiveEvent(const std::shared_ptr<Actor>& actor, float current_animation_time);

	/// <summary>
	/// �o�^����Ă���ꍇ�A�A�j���[�V�����̊J�n�C�x���g�����s����
	/// </summary>
	void ExecuteEventWhenEnter(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// �o�^����Ă���ꍇ�A�A�j���[�V�����̏I���C�x���g�����s����
	/// </summary>
	void ExecuteEventWhenExit(const std::shared_ptr<Actor>& actor);

	void InitializeAnimationEventsPerLoop();

	const size_t& GetAnimationIndex() { return animation_index; }

private:
	
	size_t animation_index;
	std::vector<std::unique_ptr<AnimationCallbackEvent>> animation_events;
	// ���̃A�j���V�����ɕύX����ۂɎ��s�����C�x���g
	std::unique_ptr<AnimationCallbackEvent> animation_event_when_enter;	
	// ���̃A�j���V������ύX����ۂɎ��s�����C�x���g
	std::unique_ptr<AnimationCallbackEvent> animation_event_when_exit;  
};
