#include "OrthographicCamera.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"
#include "Input/Input.h"

/* --- 定義 ---- */

OrthographicCamera::OrthographicCamera(const OrthographicCamera& rhs)
	: OrthographicCamera(rhs.eye, rhs.focus, rhs.near_z, rhs.far_z, rhs.is_fix_up, rhs.width, rhs.height)
{
}

OrthographicCamera::OrthographicCamera(
	const Vector3& eye,
	const Vector3& focus,
	float near_z,
	float far_z,
	bool is_fix_up,
	float width,
	float height)
	: Camera(eye, focus, near_z, far_z, is_fix_up), width(width), height(height)
{
}

void OrthographicCamera::ImGuiRender()
{
	Camera::ImGuiRender();

	float tmpwh[2]{ width,height };
	ImGui::SliderFloat2("ViewWH", tmpwh, 1.0f, 10000.0f, "%.f", ImGuiSliderFlags_Logarithmic);
	width = tmpwh[0]; height = tmpwh[1];
}

void OrthographicCamera::CalcViewProj(
	const Vector3& eye,
	const Vector3& focus,
	Matrix& out_view,
	Matrix& out_projection)
{
	out_view = Matrix::CreateLookAtLH(eye, focus, is_fix_up ? Vector3(0, 1, 0) : up);
	out_projection = Matrix::CreateOrthographic(width, height, near_z, far_z);
}
