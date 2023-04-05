#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"
#include "General/MyImGuiHelper.h"

#include "../Camera.h"

/* --- �錾 ---- */

/// <summary>
/// �J�����V�F�C�N���N���X
/// </summary>
class CameraShakeBase
{
public:

	CameraShakeBase(float shake_time)
		: time(shake_time), timer() {};
	virtual ~CameraShakeBase() = default;

	bool IsShakeEnd() const { return timer > time; }

	void ShakeTimerElapse(float elapsed_time) { timer += elapsed_time; }

	/// <summary>
	/// �J�����V�F�C�N�����邱�Ƃɂ���ĕω����鎋�_�A�����_�̒l���v�Z����B
	/// �J�����̎��_�A�����_�𒼐ڏ��������Ȃ��B
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="out_eye">�ω���̎��_</param>
	/// <param name="out_focus">�ω���̒����_</param>
	virtual void CameraShake(
		const Camera* camera,
		Vector3& out_eye,
		Vector3& out_focus) = 0;

protected:

	const float time;
	float timer;
};
