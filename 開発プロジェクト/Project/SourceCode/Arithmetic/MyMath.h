#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cmath>
#include <numbers>

#include "Arithmetic.h"

/* --- 宣言 ---- */

class Math
{
public:

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static Vector2 ScreenToClip(const Vector2& position, float screen_width, float screen_height);

	/// <summary>
	/// ワールド座標をスクリーン座標に変換する。
	/// ワールド座標をview_projection行列との積
	/// </summary>
	/// <param name="position"></param>
	/// <param name="view"></param>
	/// <param name="projection"></param>
	/// <returns></returns>
	static Vector2 WorldToScreen(
		const Vector3& position, 
		const Matrix& view, 
		const Matrix& projection, 
		float screen_width, 
		float screen_height);

	static std::pair<Vector3, Vector3> ScreenToWorld(
		const Vector2& position,
		const Matrix& view,
		const Matrix& projection,
		float screen_width,
		float screen_height);
};
