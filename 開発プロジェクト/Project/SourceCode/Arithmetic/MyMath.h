#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cmath>
#include <numbers>

#include "Arithmetic.h"

/* --- �錾 ---- */

class Math
{
public:

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static Vector2 ScreenToClip(const Vector2& position, float screen_width, float screen_height);

	/// <summary>
	/// ���[���h���W���X�N���[�����W�ɕϊ�����B
	/// ���[���h���W��view_projection�s��Ƃ̐�
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
