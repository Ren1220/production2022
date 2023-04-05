#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Camera.h"
#include "Shake/CameraShakeBase.h"

/* --- 宣言 ---- */

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
	/// ビュー行列の逆行列からカメラのワールド空間における向きベクトルを算出する
	/// </summary>
	void CalcViewProj(
		const Vector3& eye,
		const Vector3& focus,
		Matrix& out_view,
		Matrix& out_projection) override;

	float view_angle; // 視野角(Degree)
	float aspect;	// アスペクト比 (W/H)

};
