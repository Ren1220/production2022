#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "imgui.h"
#include "General/MyImGuiHelper.h"

#include "../Camera.h"

/* --- 宣言 ---- */

/// <summary>
/// カメラシェイク基底クラス
/// </summary>
class CameraShakeBase
{
public:

	CameraShakeBase(float shake_time)
		: time(shake_time), timer() {};
	virtual ~CameraShakeBase() = default;

	bool IsShakeEnd() const { return timer > time; }

	void ShakeTimerElapse(float elapsed_time) { timer += elapsed_time; }

	/// <summary>
	/// カメラシェイクをすることによって変化する視点、注視点の値を計算する。
	/// カメラの視点、注視点を直接書き換えない。
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="out_eye">変化後の視点</param>
	/// <param name="out_focus">変化後の注視点</param>
	virtual void CameraShake(
		const Camera* camera,
		Vector3& out_eye,
		Vector3& out_focus) = 0;

protected:

	const float time;
	float timer;
};
