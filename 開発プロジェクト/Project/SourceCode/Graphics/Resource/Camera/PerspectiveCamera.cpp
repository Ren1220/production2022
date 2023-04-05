#include "PerspectiveCamera.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"

/* --- 定義 ---- */

PerspectiveCamera::PerspectiveCamera(
	const Vector3& eye, 
	const Vector3& focus, 
	float near_z, 
	float far_z, 
	bool is_fix_up, 
	float view_angle,
	float aspect)
	: Camera(eye, focus, near_z, far_z, is_fix_up), view_angle(view_angle), aspect(aspect)
{
}

void PerspectiveCamera::ImGuiRender()
{
	Camera::ImGuiRender();

	ImGui::SliderFloat("ViewAngleDeg", &view_angle, 0.0f, 360.0f, "%.f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::InputFloat("Aspect", &aspect, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

void PerspectiveCamera::CalcViewProj(
	const Vector3& eye,
	const Vector3& focus,
	Matrix& out_view,
	Matrix& out_projection)
{
	out_view = Matrix::CreateLookAtLH(eye, focus, is_fix_up ? Vector3(0, 1, 0) : up);
	out_projection = Matrix::CreateProjection(view_angle, aspect, near_z, far_z);
}
