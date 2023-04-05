#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "OrthographicCamera.h"
#include "Shake/CameraShakeBase.h"

/* --- 宣言 ---- */

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

	// 注視点を軸とした回転量
	float rot_about_focus;

	float radius;
	float dist_y;

};

