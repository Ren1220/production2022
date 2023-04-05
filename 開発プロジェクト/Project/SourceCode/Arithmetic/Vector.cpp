#include "Vector.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <algorithm>
#include "Matrix.h"

/* --- 定義 ---- */

float Vector3::Dot(const Vector3& vector3) const
{
	return x * vector3.x + y * vector3.y + z * vector3.z;
}

float Vector3::Dot(const Vector4& vector4) const
{
	return x * vector4.x + y * vector4.y + z * vector4.z + vector4.w;
}

Vector3 Vector3::Cross(const Vector3& rhs) const
{
	return { y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x };
}

float Vector3::LengthSquare() const
{
	return x * x + y * y + z * z;
}

float Vector3::Length() const
{
	return sqrtf(LengthSquare());
}

float Vector3::Distance(const Vector3& vector3) const
{
	return (vector3 - *this).Length();
}

Vector3 Vector3::Normalize() const
{
	return *this / Length();
}

bool Vector3::NearEqual(const Vector3& vector3, const Vector3& rounding) const
{
	_ASSERT_EXPR(rounding.x > 0 && rounding.y > 0 && rounding.z > 0, L"誤差の範囲の値が不正です");
	return abs(x - vector3.x) < rounding.x && abs(y - vector3.y) < rounding.y && abs(z - vector3.z) < rounding.z;
}

bool Vector3::IsZero() const
{
	return x == 0 && y == 0 && z == 0;
}

Vector3 Vector3::Transform(const Matrix& transform_matrix) const
{
	// https://www.headboost.jp/what-is-linear-transformation/
	// https://blog.natade.net/2017/06/11/vector-matrix-directx-opengl-hlsl-glsl/

	// DirectX -> v * M // | (vx + M11) + (vy * M21) + (vz * M31) + M41 |
						// | (vx + M12) + (vy * M22) + (vz * M32) + M42 |
						// | (vx + M13) + (vy * M23) + (vz * M33) + M43 |
						// | (vx + M14) + (vy * M24) + (vz * M34) + M44 |

	return
	{
		x * transform_matrix._11 + y * transform_matrix._21 + z * transform_matrix._31 + transform_matrix._41,
		x * transform_matrix._12 + y * transform_matrix._22 + z * transform_matrix._32 + transform_matrix._42,
		x * transform_matrix._13 + y * transform_matrix._23 + z * transform_matrix._33 + transform_matrix._43
	};
}

Vector3 Vector3::TransformCoordinate(const Matrix& transform_matrix) const
{
	float w = x * transform_matrix._14 + y * transform_matrix._24 + z * transform_matrix._34 + transform_matrix._44;

	return
	{
		(x * transform_matrix._11 + y * transform_matrix._21 + z * transform_matrix._31 + transform_matrix._41) / w,
		(x * transform_matrix._12 + y * transform_matrix._22 + z * transform_matrix._32 + transform_matrix._42) / w,
		(x * transform_matrix._13 + y * transform_matrix._23 + z * transform_matrix._33 + transform_matrix._43) / w
	};
}

Vector3 Vector3::TransformNormal(const Matrix& transform_matrix) const
{
	return
	{
		x * transform_matrix._11 + y * transform_matrix._21 + z * transform_matrix._31,
		x * transform_matrix._12 + y * transform_matrix._22 + z * transform_matrix._32,
		x * transform_matrix._13 + y * transform_matrix._23 + z * transform_matrix._33
	};
}

Vector3 Vector3::Lerp(const Vector3& vector3, float interpolated) const
{
	// V0 + t * (V1 - V0)
	return *this + interpolated * (vector3 - *this);
}

Vector3 Vector3::Clamp(const Vector3& min, const Vector3& max) const
{
	return
	{
		std::clamp(x, min.x, max.x),
		std::clamp(y, min.y, max.y),
		std::clamp(z, min.z, max.z)
	};
}

