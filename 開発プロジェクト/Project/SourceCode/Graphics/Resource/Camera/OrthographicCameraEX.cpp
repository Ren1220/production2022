#include "OrthographicCameraEX.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Input/Input.h"

/* --- 定義 ---- */

OrthographicCameraEX::OrthographicCameraEX(const OrthographicCameraEX& rhs)
	: OrthographicCameraEX(rhs.eye, rhs.focus, rhs.near_z, rhs.far_z, rhs.is_fix_up, rhs.width, rhs.height)
{
}

OrthographicCameraEX::OrthographicCameraEX(
	const Vector3& eye,
	const Vector3& focus,
	float near_z,
	float far_z,
	bool is_fix_up,
	float width,
	float height)
	: OrthographicCamera(eye, focus, near_z, far_z, is_fix_up, width, height),
	  rot_about_focus(4.71f), radius(20.0f), dist_y(20.0f)

{
}

void OrthographicCameraEX::ImGuiRender()
{
}

void OrthographicCameraEX::Control(float elapsed_time)
{
	const auto& input{ Input::GetInstance() };
	const auto& game_pad{ input.GetGamePad() };

	if (game_pad.IsConnectingController())
	{
		// コントローラーが接続されている

		float rx{ game_pad.GetStickRX() };
		rot_about_focus += rx * elapsed_time * 2.0f;

		eye.x = radius * cosf(rot_about_focus) - radius * sinf(rot_about_focus);
		eye.z = radius * sinf(rot_about_focus) + radius * cosf(rot_about_focus);
		eye.y = dist_y;

		eye += focus;
	}
}

void OrthographicCameraEX::Zoom(float width, float height, float t)
{
	this->width = std::lerp(this->width, width, t);
	this->height = std::lerp(this->height, height, t);
}
