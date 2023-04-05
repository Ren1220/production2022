#include "Camera.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"

#include "Shake/CameraShakeBase.h"

/* --- ��` ---- */

Camera::Camera(
	const Vector3& eye,
	const Vector3& focus,
	float near_z,
	float far_z,
	bool is_fix_up)
	: view(), projection(), 
	  eye(eye), focus(focus), 
	  right(), up(0, 1, 0), forward(), 
	  near_z(near_z), far_z(far_z),
	  is_fix_up(is_fix_up)
{

}

void Camera::Update(float elapsed_time)
{

	Control(elapsed_time);

	// �J�����V�F�C�N�ł����̒l�𒼐ڏ��������Ȃ��悤�ɂ���B
	Vector3 e{ eye };
	Vector3 f{ focus };

	// �J�����V�F�C�N���s���ꍇ�A���̊֐����Ŏ��_�E�����_������������B
	CameraShake(elapsed_time, e, f);

	CalcViewProj(e, f, view, projection);
	CalcDirection();
}

void Camera::ImGuiRender()
{
	ImGui::InputFloat3("Eye", &eye.x);
	ImGui::InputFloat3("Focus", &focus.x);

	ImGui::InputFloat3("Right", &right.x, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Up", &up.x, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Forward", &forward.x, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	ImGui::Checkbox("IsFixUp", &is_fix_up);

	ImGui::SliderFloat("Near", &near_z, 0.001f, far_z, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("Far", &far_z, near_z, 10000.0f, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
}

void Camera::SetCameraShake(std::unique_ptr<CameraShakeBase>&& camera_shake)
{
	this->camera_shake = std::move(camera_shake);
}

void Camera::CalcDirection()
{
	auto vi{ view.Inverse() };
	right = vi.m[0]; up = vi.m[1]; forward = vi.m[2];
}

void Camera::CameraShake(
	float elapsed_time, 
	Vector3& out_eye,
	Vector3& out_focus)
{
	if (camera_shake)
	{
		// �J�����V�F�C�N���s���ω������A���_�E�����_���擾����
		camera_shake->CameraShake(this, out_eye, out_focus);

		camera_shake->ShakeTimerElapse(elapsed_time);

		// �J�����V�F�C�N���I������j��
		if (camera_shake->IsShakeEnd())
		{
			camera_shake.reset();
		}
	}
}
