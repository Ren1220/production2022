#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Camera.h"
#include "Shake/CameraShakeBase.h"

/* --- �錾 ---- */

class OrthographicCamera : public Camera
{
public:

	OrthographicCamera(const OrthographicCamera& rhs);
	OrthographicCamera(
		const Vector3& eye,
		const Vector3& focus,
		float near_z,
		float far_z,
		bool is_fix_up,
		float width,
		float height);
	virtual ~OrthographicCamera() = default;

	void ImGuiRender() override;

	float GetWidth() { return width; }
	float GetHeight() { return height; }

	void SetWidth(float width) { this->width = width; }
	void SetHeight(float height) { this->height = height; }

protected:

	/// <summary>
	/// �r���[�s��̋t�s�񂩂�J�����̃��[���h��Ԃɂ���������x�N�g�����Z�o����
	/// </summary>
	void CalcViewProj(
		const Vector3& eye,
		const Vector3& focus,
		Matrix& out_view,
		Matrix& out_projection) override;

	float width;
	float height;

};
