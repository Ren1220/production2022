#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "OrthographicCamera.h"
#include "Shake/CameraShakeBase.h"

/* --- �錾 ---- */

class OrthographicCameraEX : public OrthographicCamera
{
public:

	OrthographicCameraEX(const OrthographicCameraEX& rhs);
	OrthographicCameraEX(
		const Vector3& eye,
		const Vector3& focus,
		float near_z,
		float far_z,
		bool is_fix_up,
		float width,
		float height);

	virtual ~OrthographicCameraEX() = default;

	void ImGuiRender() override;

	void Control(float elapsed_time) override;

	void Zoom(float width, float height, float t);

private:

	// �����_�����Ƃ�����]��
	float rot_about_focus;

	float radius;
	float dist_y;

};

