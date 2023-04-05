#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <optional>

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "Animation.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

/// <summary>
/// �A�j���[�V�����R���|�[�l���g
/// </summary>
class Animator
{
public:

	Animator();
	~Animator() = default;

	void Start();
	void End();
	void Update(float elapsed_time);
	void ImGuiRender();

	/// <summary>
	/// ���݂̃A�j���[�V�������Ԃɉ������Đ����̃A�j���[�V�����̃C�x���g�����s����
	/// </summary>
	void AnimationEvent();

	/// <summary>
	/// �A�j���V�����̓o�^
	/// </summary>
	/// <param name="key">�ݒ肷��L�[</param>
	/// <param name="filepath">�o�^����A�j���[�V�����̃C���f�b�N�X</param>
	const std::shared_ptr<Animation>& RegisterAnimation(uint32_t key, size_t animation_index);

	/// <summary>
	/// <para>�o�^�����A�j���[�V�������L�[�ŌĂяo���A�Đ�����B</para>
	/// <para>�Đ�����A�j���[�V�����ɊJ�n���R�[���o�b�N�C�x���g���o�^����Ă���ꍇ���s������B</para>
	/// <para>���A�O��Đ����̃A�j���[�V���������݂��A���̏I�����R�[���o�b�N�C�x���g���o�^����Ă���ꍇ�A�n�߂Ɏ��s������</para>
	/// <para>��O�����ŕ�ԑ��x��ݒ肷�邱�ƂŃA�j���[�V�����u�����h�ł��邪�A�����o�O�����肻���Ȃ̂Œ��ӂ���i���ɃA�j���[�V�����C�x���g�̌��ˍ���������j</para>
	/// </summary>
	/// <param name="key">�Đ�����A�j���[�V�����̃L�[</param>
	/// <param name="is_loop">���[�v�Đ����s����</param>
	/// <param name="interpolation">�u�����h��ԑ��x�BFLT_MAX�̎��̓u�����h���Ȃ�</param>
	void PlayAnimation(uint32_t key, bool is_loop, float interpolation = FLT_MAX);

	/// <summary>
	/// <para>���ݍĐ����̃A�j���[�V�������I��������B					</para>
	/// <para>�I�����R�[���o�b�N�C�x���g���o�^����Ă���ꍇ���s������B</para>
	/// </summary>
	void StopAnimation();

	bool IsPlayAnimation() const { return !has_played_animation; } // �A�j���[�V�����Đ�����

	bool HasPlayedAnimation() const { return has_played_animation; }

	std::shared_ptr<Animation> GetCurrentPlayAnimation() const;

	const std::shared_ptr<Animation>& GetAnimation(uint32_t key) const;

	size_t CountOfRegisteredAnimation() const { return registered_animations.size(); }

	void SetActor(const std::shared_ptr<Actor>& actor);
	std::shared_ptr<Actor> GetActor() const;

	float GetAnimBlendRatio() const { return blend_ratio; }
	void SetAnimBlendRatio(float f) { blend_ratio = f; }

private:

	/// <summary>
	/// �o�^����Ă���A�j���[�V�����̃L�[�ƈ�v����A�j���[�V������ݒ肷��
	/// </summary>
	/// <param name="key"></param>
	void SetCurrentAnimation(uint32_t key);

private:

	std::unordered_map<uint32_t, std::shared_ptr<Animation>> registered_animations;
	std::weak_ptr<Actor> w_actor;

	std::optional<std::unordered_map<uint32_t, std::shared_ptr<Animation>>::const_iterator> current_animation;
	std::optional<std::unordered_map<uint32_t, std::shared_ptr<Animation>>::const_iterator> next_animation;

	float interpolation;
	float blend_ratio;

	float animation_timer1;				// �A�j���[�V�����^�C�}�[
	float animation_timer2;				// �A�j���[�V�����^�C�}�[
	bool is_loop;						// ���[�v�Đ����s����
	bool has_looped;					// ���[�v�Đ����s������
	bool has_played_animation;			// �A�j���[�V�������Đ�������
};
