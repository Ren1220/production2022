#include "Matrix.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Quaternion.h"

/* --- 定義 ---- */

Matrix Matrix::CreateLookAtRH(const Vector3& eye, const Vector3& target, const Vector3& up)
{

	// カメラの注視点の座標からカメラ座標を引いてZ軸のベクトルを求める
	Vector3 z { (target - eye).Normalize() };

	// 求めたZ軸ベクトルとカメラの上方向の外積でX軸ベクトルを求める
	Vector3 x{ up.Cross(z).Normalize() };

	// 求めたX軸ベクトルとZ軸ベクトルの外積でY軸ベクトルを求める
	Vector3 y{ z.Cross(x) };

	// カメラの座標とZ軸ベクトルから移動量を求める
	Vector3 p{ eye.Dot(x) ,eye.Dot(y) ,eye.Dot(z) };

	return
	{
		 x.x, x.y, x.z, 0,
		 y.x, y.y, y.z, 0,
		 z.x, z.y, z.z, 0,
		 p.x, p.y, p.z, 1
	};
}

Matrix Matrix::CreateLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	// カメラの注視点の座標からカメラ座標を引いてZ軸のベクトルを求める
	Vector3 z{ (target - eye).Normalize() };

	// 求めたZ軸ベクトルとカメラの上方向の外積でX軸ベクトルを求める
	Vector3 x{ up.Cross(z).Normalize() };

	// 求めたX軸ベクトルとZ軸ベクトルの外積でY軸ベクトルを求める
	Vector3 y{ z.Cross(x) };

	// カメラの座標とZ軸ベクトルから移動量を求める
	Vector3 p{ eye.Dot(x) ,eye.Dot(y) ,eye.Dot(z) };

	return
	{
		 x.x,  y.x,  z.x, 0,
		 x.y,  y.y,  z.y, 0,
		 x.z,  y.z,  z.z, 0,
		-p.x, -p.y, -p.z, 1,
	};
}

Matrix Matrix::CreateProjection(float view_angle, float screen_width, float screen_height, float near_z, float far_z)
{
	// https://yttm-work.jp/gmpg/gmpg_0004.html

	view_angle = view_angle * 3.14f / 180.0f;

	float tan{ 1.0f / tanf(view_angle / 2.0f) };

	return
	{
		tan / (screen_width / screen_height), 0, 0, 0,
		0, tan, 0, 0,
		0, 0, 1 / (far_z - near_z) * far_z, 1,
		0, 0, -near_z / (far_z - near_z) * far_z, 0
	};
}

Matrix Matrix::CreateProjection(float view_angle, float aspect, float near_z, float far_z)
{
	// https://yttm-work.jp/gmpg/gmpg_0004.html

	view_angle = view_angle * 3.14f / 180.0f;

	float tan{ 1.0f / tanf(view_angle / 2.0f) };

	return
	{
		tan / aspect, 0, 0, 0,
		0, tan, 0, 0,
		0, 0, 1 / (far_z - near_z) * far_z, 1,
		0, 0, -near_z / (far_z - near_z) * far_z, 0
	};
}

Matrix Matrix::CreateOrthographic(float screen_width, float screen_height, float near_z, float far_z)
{
	float range{ 1.0f / (far_z - near_z) };

	return
	{
		2.0f / screen_width, 0, 0, 0,
		0, 2.0f / screen_height, 0, 0,
		0, 0, range, 0,
		0, 0, -range * near_z, 1
	};
}

Matrix Matrix::CreateTransform(const Vector3& translation, const Quaternion& rotation, const Vector3& scaling)
{
	// S * R * T
#if 1
	float length = sqrtf(rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z + rotation.w * rotation.w);
	length = 2.0f / (length * length);

	return
	{
		(1.0f - rotation.y * rotation.y * length - rotation.z * rotation.z * length) * scaling.x,
		(rotation.x * rotation.y * length + rotation.z * rotation.w * length) * scaling.x,
		(rotation.z * rotation.x * length - rotation.y * rotation.w * length) * scaling.x,
		0.0f,
		(rotation.x * rotation.y * length - rotation.z * rotation.w * length) * scaling.y,
		(1.0f - rotation.x * rotation.x * length - rotation.z * rotation.z * length) * scaling.y,
		(rotation.y * rotation.z * length + rotation.x * rotation.w * length) * scaling.y,
		0.0f,
		(rotation.z * rotation.x * length + rotation.y * rotation.w * length) * scaling.z,
		(rotation.y * rotation.z * length - rotation.x * rotation.w * length) * scaling.z,
		(1.0f - rotation.x * rotation.x * length - rotation.y * rotation.y * length) * scaling.z,
		0.0f,
		translation.x, translation.y, translation.z, 1.0f						  
	};

#else
	return CreateScaling(scaling) * CreateRotation(rotation) * CreateTranslation(translation);
#endif

}

Matrix Matrix::CreateTransform(const Vector3& translation, const Vector3& rotation, const Vector3& scaling)
{
	// S * R * T
#if 1
	return
	{
		(cosf(rotation.y) * cosf(rotation.z) + sinf(rotation.x) * sinf(rotation.y) * sinf(rotation.z)) * scaling.x,  // _11
		(cosf(rotation.x) * sinf(rotation.z)) * scaling.x,															 // _12
		(-sinf(rotation.y) * cosf(rotation.z) + sinf(rotation.x) * cosf(rotation.y) * sinf(rotation.z)) * scaling.x, // _13
		0,																											 // _14
		(-cosf(rotation.y) * sinf(rotation.z) + sinf(rotation.x) * sinf(rotation.y) * cosf(rotation.z)) * scaling.y, // _21
		(cosf(rotation.x) * cosf(rotation.z)) * scaling.y,															 // _22
		(sinf(rotation.y) * sinf(rotation.z) + sinf(rotation.x) * cosf(rotation.y) * cosf(rotation.z)) * scaling.y,  // _23
		0,																											 // _24
		(cosf(rotation.x) * sinf(rotation.y)) * scaling.z,															 // _31
		(-sinf(rotation.x)) * scaling.z,																			 // _32
		(cosf(rotation.x) * cosf(rotation.y))* scaling.z,															 // _33
		0,																											 // _34
		translation.x, translation.y, translation.z, 1																 // _4X
	};
#else
	return CreateScaling(scaling) * CreateRotation(rotation) * CreateTranslation(translation);
#endif
}

Matrix Matrix::CreateTranslation(const Vector3& translation)
{
	return
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translation.x, translation.y, translation.z, 1
	};
}

Matrix Matrix::CreateRotation(const Quaternion& rotation)
{
	float length = sqrtf(rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z + rotation.w * rotation.w);
	length = 2.0f / (length * length);

	return
	{
		1.0f - rotation.y * rotation.y * length - rotation.z * rotation.z * length,
		rotation.x * rotation.y * length + rotation.z * rotation.w * length,
		rotation.z * rotation.x * length - rotation.y * rotation.w * length,
		0.0f,
		rotation.x * rotation.y * length - rotation.z * rotation.w * length,
		1.0f - rotation.x * rotation.x * length - rotation.z * rotation.z * length,
		rotation.y * rotation.z * length + rotation.x * rotation.w * length,
		0.0f,
		rotation.z * rotation.x * length + rotation.y * rotation.w * length,
		rotation.y * rotation.z * length - rotation.x * rotation.w * length,
		1.0f - rotation.x * rotation.x * length - rotation.y * rotation.y * length,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix Matrix::CreateRotation(const Vector3& rotation)
{
	// https://kamino.hatenablog.com/entry/rotation_expressions#sec3_2

	// Z *( X * Y )

#if 1
	return
	{
		cosf(rotation.y) * cosf(rotation.z) + sinf(rotation.x) * sinf(rotation.y) * sinf(rotation.z),
		cosf(rotation.x) * sinf(rotation.z),
		-sinf(rotation.y) * cosf(rotation.z) + sinf(rotation.x)	* cosf(rotation.y) * sinf(rotation.z),
		0,
		-cosf(rotation.y) * sinf(rotation.z) + sinf(rotation.x) * sinf(rotation.y) * cosf(rotation.z),
		cosf(rotation.x) * cosf(rotation.z),
		sinf(rotation.y) * sinf(rotation.z) + sinf(rotation.x) * cosf(rotation.y) * cosf(rotation.z),
		0,
		cosf(rotation.x) * sinf(rotation.y),
		-sinf(rotation.x),
		cosf(rotation.x) * cosf(rotation.y),
		0,
		0, 0, 0, 1
	};
#else
	return
	{
		CreateRotationZ(rotation.z) * CreateRotationX(rotation.x) * CreateRotationY(rotation.y)
	};
#endif

}

Matrix Matrix::CreateRotationX(float rotation_x)
{
	// 転置済み
	return
	{
		1, 0, 0, 0,
		0, cosf(rotation_x), sinf(rotation_x), 0,
		0, -sinf(rotation_x), cosf(rotation_x), 0,
		0, 0, 0, 1
	};
}

Matrix Matrix::CreateRotationY(float rotation_y)
{
	// 転置済み
	return
	{
		cosf(rotation_y), 0, -sinf(rotation_y), 0,
		0, 1, 0, 0,
		sinf(rotation_y), 0, cosf(rotation_y), 0,
		0, 0, 0, 1
	};
}

Matrix Matrix::CreateRotationZ(float rotation_z)
{
	// 転置済み
	return
	{
		cosf(rotation_z), sinf(rotation_z), 0, 0,
		-sinf(rotation_z), +cosf(rotation_z), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

Matrix Matrix::CreateScaling(const Vector3& scaling)
{
	return
	{
		scaling.x, 0, 0, 0,
		0, scaling.y, 0, 0,
		0, 0, scaling.z, 0,
		0, 0, 0, 1
	};
}

Matrix Matrix::Inverse() const
{
	// http://www.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/tech0023.html

	float determinant =
		_11 * ((_22 * _33 * _44) + (_23 * _34 * _42) + (_24 * _32 * _43) - (_22 * _34 * _43) - (_23 * _32 * _44) - (_24 * _33 * _42)) +
		_12 * ((_21 * _34 * _43) + (_23 * _31 * _44) + (_24 * _33 * _41) - (_21 * _33 * _44) - (_23 * _34 * _41) - (_24 * _31 * _43)) +
		_13 * ((_21 * _32 * _44) + (_22 * _34 * _41) + (_24 * _31 * _42) - (_21 * _34 * _42) - (_22 * _31 * _44) - (_24 * _32 * _41)) +
		_14 * ((_21 * _33 * _42) + (_22 * _31 * _43) + (_23 * _32 * _41) - (_21 * _32 * _43) - (_22 * _33 * _41) - (_23 * _31 * _42));


	determinant = 1.0f / determinant;

	return
	{
		((_22 * _33 * _44) + (_23 * _34 * _42) + (_24 * _32 * _43) - (_22 * _34 * _43) - (_23 * _32 * _44) - (_24 * _33 * _42)) * determinant,
		((_12 * _34 * _43) + (_13 * _32 * _44) + (_14 * _33 * _42) - (_12 * _33 * _44) - (_13 * _34 * _42) - (_14 * _32 * _43)) * determinant,
		((_12 * _23 * _44) + (_13 * _24 * _42) + (_14 * _22 * _43) - (_12 * _24 * _43) - (_13 * _22 * _44) - (_14 * _23 * _42)) * determinant,
		((_12 * _24 * _33) + (_13 * _22 * _34) + (_14 * _23 * _32) - (_12 * _23 * _34) - (_13 * _24 * _32) - (_14 * _22 * _33)) * determinant,
		((_21 * _34 * _43) + (_23 * _31 * _44) + (_24 * _33 * _41) - (_21 * _33 * _44) - (_23 * _34 * _41) - (_24 * _31 * _43)) * determinant,
		((_11 * _33 * _44) + (_13 * _34 * _41) + (_14 * _31 * _43) - (_11 * _34 * _43) - (_13 * _31 * _44) - (_14 * _33 * _41)) * determinant,
		((_11 * _24 * _43) + (_13 * _21 * _44) + (_14 * _23 * _41) - (_11 * _23 * _44) - (_13 * _24 * _41) - (_14 * _21 * _43)) * determinant,
		((_11 * _23 * _34) + (_13 * _24 * _31) + (_14 * _21 * _33) - (_11 * _24 * _33) - (_13 * _21 * _34) - (_14 * _23 * _31)) * determinant,
		((_21 * _32 * _44) + (_22 * _34 * _41) + (_24 * _31 * _42) - (_21 * _34 * _42) - (_22 * _31 * _44) - (_24 * _32 * _41)) * determinant,
		((_11 * _34 * _42) + (_12 * _31 * _44) + (_14 * _32 * _41) - (_11 * _32 * _44) - (_12 * _34 * _41) - (_14 * _31 * _42)) * determinant,
		((_11 * _22 * _44) + (_12 * _24 * _41) + (_14 * _21 * _42) - (_11 * _24 * _42) - (_12 * _21 * _44) - (_14 * _22 * _41)) * determinant,
		((_11 * _24 * _32) + (_12 * _21 * _34) + (_14 * _22 * _31) - (_11 * _22 * _34) - (_12 * _24 * _31) - (_14 * _21 * _32)) * determinant,
		((_21 * _33 * _42) + (_22 * _31 * _43) + (_23 * _32 * _41) - (_21 * _32 * _43) - (_22 * _33 * _41) - (_23 * _31 * _42)) * determinant,
		((_11 * _32 * _43) + (_12 * _33 * _41) + (_13 * _31 * _42) - (_11 * _33 * _42) - (_12 * _31 * _43) - (_13 * _32 * _41)) * determinant,
		((_11 * _23 * _42) + (_12 * _21 * _43) + (_13 * _22 * _41) - (_11 * _22 * _43) - (_12 * _23 * _41) - (_13 * _21 * _42)) * determinant,
		((_11 * _22 * _33) + (_12 * _23 * _31) + (_13 * _21 * _32) - (_11 * _23 * _32) - (_12 * _21 * _33) - (_13 * _22 * _31)) * determinant
	};
}

void Matrix::DecomposeTransform(Vector3& out_translation, Quaternion& out_rotation, Vector3& out_scaling) const
{
	// http://marupeke296.sakura.ne.jp/DXG_No39_WorldMatrixInformation.html

	// 平行移動量の抽出
	out_translation = { _41, _42, _43 };

	// スケーリングの抽出
	out_scaling =
	{
		sqrtf(_11 * _11 + _12 * _12 + _13 * _13),
		sqrtf(_21 * _21 + _22 * _22 + _23 * _23),
		sqrtf(_31 * _31 + _32 * _32 + _33 * _33)
	};

	// 回転行列を抽出して、クォータニオンに変換する
	out_rotation = Quaternion( 
		{
			_11 / out_scaling.x, _12 / out_scaling.x, _13 / out_scaling.x, 0.0f,
			_21 / out_scaling.y, _22 / out_scaling.y, _23 / out_scaling.y, 0.0f,
			_31 / out_scaling.z, _32 / out_scaling.z, _33 / out_scaling.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		});

}

void Matrix::DecomposeTransform(Vector3& out_translation, Matrix& out_rotation, Vector3& out_scaling) const
{
	// http://marupeke296.sakura.ne.jp/DXG_No39_WorldMatrixInformation.html

	// 平行移動量の抽出
	out_translation = { _41, _42, _43 };

	// スケーリングの抽出
	out_scaling =
	{
		sqrtf(_11 * _11 + _12 * _12 + _13 * _13),
		sqrtf(_21 * _21 + _22 * _22 + _23 * _23),
		sqrtf(_31 * _31 + _32 * _32 + _33 * _33)
	};

	// 回転行列を抽出して、クォータニオンに変換する
	out_rotation = 
	{
		_11 / out_scaling.x, _12 / out_scaling.x, _13 / out_scaling.x, 0.0f,
		_21 / out_scaling.y, _22 / out_scaling.y, _23 / out_scaling.y, 0.0f,
		_31 / out_scaling.z, _32 / out_scaling.z, _33 / out_scaling.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

std::tuple<Vector3, Quaternion, Vector3> Matrix::DecomposeTransform() const
{
	Vector3 translation;
	Quaternion rotation;
	Vector3 scale;

	DecomposeTransform(translation, rotation, scale);

	return { translation, rotation, scale };
}

#if 0
Matrix Inverse() const
{
	Matrix b;
	b._11 = _22 * (_33 * _44 - _34 * _43) + _23 * (_34 * _42 - _32 * _44) + _24 * (_32 * _43 - _33 * _42);
	b._12 = _32 * (_44 * _13 - _43 * _14) + _33 * (_42 * _14 - _44 * _12) + _34 * (_43 * _12 - _42 * _13);
	b._13 = _42 * (_13 * _24 - _14 * _23) + _43 * (_14 * _22 - _12 * _24) + _44 * (_12 * _23 - _13 * _22);
	b._14 = _12 * (_24 * _33 - _23 * _34) + _13 * (_22 * _34 - _24 * _32) + _14 * (_23 * _32 - _22 * _33);

	b._21 = _23 * (_31 * _44 - _34 * _41) + _24 * (_33 * _41 - _31 * _43) + _21 * (_34 * _43 - _33 * _44);
	b._22 = _33 * (_44 * _11 - _41 * _14) + _34 * (_41 * _13 - _43 * _11) + _31 * (_43 * _14 - _44 * _13);
	b._23 = _43 * (_11 * _24 - _14 * _21) + _44 * (_13 * _21 - _11 * _23) + _41 * (_14 * _23 - _13 * _24);
	b._24 = _13 * (_24 * _31 - _21 * _34) + _14 * (_21 * _33 - _23 * _31) + _11 * (_23 * _34 - _24 * _33);

	b._31 = _24 * (_31 * _42 - _32 * _41) + _21 * (_32 * _44 - _34 * _42) + _22 * (_34 * _41 - _31 * _44);
	b._32 = _34 * (_42 * _11 - _41 * _12) + _31 * (_44 * _12 - _42 * _14) + _32 * (_41 * _14 - _44 * _11);
	b._33 = _44 * (_11 * _22 - _12 * _21) + _41 * (_12 * _24 - _14 * _22) + _42 * (_14 * _21 - _11 * _24);
	b._34 = _14 * (_22 * _31 - _21 * _32) + _11 * (_24 * _32 - _22 * _34) + _12 * (_21 * _34 - _24 * _31);

	b._41 = _21 * (_33 * _42 - _32 * _43) + _22 * (_31 * _43 - _33 * _41) + _23 * (_32 * _41 - _31 * _42);
	b._42 = _31 * (_42 * _13 - _43 * _12) + _32 * (_43 * _11 - _41 * _13) + _33 * (_41 * _12 - _42 * _11);
	b._43 = _41 * (_13 * _22 - _12 * _23) + _42 * (_11 * _23 - _13 * _21) + _43 * (_12 * _21 - _11 * _22);
	b._44 = _11 * (_22 * _33 - _23 * _32) + _12 * (_23 * _31 - _21 * _33) + _13 * (_21 * _32 - _22 * _31);

	float det = 1.0f / (_11 * b._11 + _21 * b._12 + _31 * b._13 + _41 * b._14);

	return
	{
		b._11 * det, b._12 * det, b._13 * det, b._14 * det,
		b._21 * det, b._22 * det, b._23 * det, b._24 * det,
		b._31 * det, b._32 * det, b._33 * det, b._34 * det,
		b._41 * det, b._42 * det, b._43 * det, b._44 * det
	};
}
#endif