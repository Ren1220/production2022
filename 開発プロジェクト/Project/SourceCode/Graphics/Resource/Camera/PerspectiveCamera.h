#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Camera.h"
#include "Shake/CameraShakeBase.h"

/* --- �錾 ---- */

class PerspectiveCamera : public Camera
{
public:

	PerspectiveCamera(
		const Vector3& eye,
		const Vector3& focus,
		float near_z,
		float far_z,
		bool is_fix_up,
		float view_angle,
		float aspect);
	virtual ~PerspectiveCamera() = default;

	void ImGuiRender() override;

	void SetViewAngle(float view_angle) { this->view_angle = view_angle; }
	void SetAspect(float aspect) { this->aspect = aspect; }

private:

	/// <summary>
	/// �r���[�s��̋t�s�񂩂�J�����̃��[���h��Ԃɂ���������x�N�g�����Z�o����
	/// </summary>
	void CalcViewProj(
		const Vector3& eye,
		const Vector3& focus,
		Matrix& out_view,
		Matrix& out_projection) override;

	float view_angle; // ����p(Degree)
	float aspect;	// �A�X�y�N�g�� (W/H)

};
