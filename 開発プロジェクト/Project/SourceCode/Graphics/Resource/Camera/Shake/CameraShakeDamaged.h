#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "CameraShakeBase.h"

/* --- �錾 ---- */

class CameraShakeDamaged : public CameraShakeBase
{
public:

	/// <summary>
	/// �_���[�W�����������ۂ̃J�����V�F�C�N
	/// </summary>
	/// <param name="shake_time"></param>
	/// <param name="amplitude">�h��̑傫��</param>
	/// <param name="frequency">�h��̑���</param>
	CameraShakeDamaged(float shake_time, float amplitude, float frequency)
		: CameraShakeBase(shake_time), amplitude(amplitude), frequency(frequency) {};
	virtual ~CameraShakeDamaged() = default;

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
		Vector3& out_focus) override;
private:

	float amplitude; // �U��
	float frequency; // �h��̑���
};
