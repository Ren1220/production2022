#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "CameraShakeBase.h"

/* --- 宣言 ---- */

class CameraShakeDamaged : public CameraShakeBase
{
public:

	/// <summary>
	/// ダメージが発生した際のカメラシェイク
	/// </summary>
	/// <param name="shake_time"></param>
	/// <param name="amplitude">揺れの大きさ</param>
	/// <param name="frequency">揺れの速さ</param>
	CameraShakeDamaged(float shake_time, float amplitude, float frequency)
		: CameraShakeBase(shake_time), amplitude(amplitude), frequency(frequency) {};
	virtual ~CameraShakeDamaged() = default;

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
		Vector3& out_focus) override;
private:

	float amplitude; // 振幅
	float frequency; // 揺れの速さ
};
