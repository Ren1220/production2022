#include "Quaternion.h"

/* --- 追加のヘッダファイルのインクルード ---- */

/* --- 定義 ---- */

Quaternion::Quaternion(const Vector3& axis, float angle)
{
	Vector3 normalized_axis{ axis.Normalize() };

	float sin{ sinf(angle * 0.5f) };

	*this =
	{
		normalized_axis.x * sin,
		normalized_axis.y * sin,
		normalized_axis.z * sin,
		cosf(angle * 0.5f)
	};
}

Quaternion::Quaternion(const Matrix& rotation)
	: Quaternion()
{
	// http://marupeke296.sakura.ne.jp/DXG_No58_RotQuaternionTrans.html

	// 最大成分を検索
	float elem[4] {}; // 0:x, 1:y, 2:z, 3:w
	elem[0] = rotation._11 - rotation._22 - rotation._33 + 1.0f;
	elem[1] = -rotation._11 + rotation._22 - rotation._33 + 1.0f;
	elem[3] = rotation._11 + rotation._22 + rotation._33 + 1.0f;
	elem[2] = -rotation._11 - rotation._22 + rotation._33 + 1.0f;

	unsigned biggestIndex = 0;
	for (int i = 1; i < 4; i++)
	{
		if (elem[i] > elem[biggestIndex])
		{
			biggestIndex = i;
		}
	}

	if (elem[biggestIndex] < 0.0f)
	{
		// 作成失敗
		x = 0;
		y = 0;
		z = 0;
		w = 1;
		return;
	}

	// 最大要素の値を算出
	float* q[4]      = { &x, &y, &z, &w };
	float v          = sqrtf(elem[biggestIndex]) * 0.5f;
	*q[biggestIndex] = v;
	float mult       = 0.25f / v;

	switch (biggestIndex)
	{
		case 0: // x
			*q[1] = (rotation._12 + rotation._21) * mult;
			*q[2] = (rotation._31 + rotation._13) * mult;
			*q[3] = (rotation._23 - rotation._32) * mult;
			break;
		case 1: // y
			*q[0] = (rotation._12 + rotation._21) * mult;
			*q[2] = (rotation._23 + rotation._32) * mult;
			*q[3] = (rotation._31 - rotation._13) * mult;
			break;
		case 2: // z
			*q[0] = (rotation._31 + rotation._13) * mult;
			*q[1] = (rotation._23 + rotation._32) * mult;
			*q[3] = (rotation._12 - rotation._21) * mult;
			break;
		case 3: // w
			*q[0] = (rotation._23 - rotation._32) * mult;
			*q[1] = (rotation._31 - rotation._13) * mult;
			*q[2] = (rotation._12 - rotation._21) * mult;
			break;
	}
}

Quaternion Quaternion::Inverse() const
{
	return Conjugate() / (sqrtf(x * x + y + y * z * z + w * w));
}

Quaternion Quaternion::Multiply(const Quaternion& quaternion) const
{
	// [ (Q2.w * Q1.x) + (Q2.x * Q1.w) + (Q2.y * Q1.z) - (Q2.z * Q1.y),
	//   (Q2.w * Q1.y) - (Q2.x * Q1.z) + (Q2.y * Q1.w) + (Q2.z * Q1.x),
	//   (Q2.w * Q1.z) + (Q2.x * Q1.y) - (Q2.y * Q1.x) + (Q2.z * Q1.w),
	//   (Q2.w * Q1.w) - (Q2.x * Q1.x) - (Q2.y * Q1.y) - (Q2.z * Q1.z) ]

	// 虚部ベクトルの内積と外積を用いて計算する
	return
	{
		(quaternion.w * x) + (quaternion.x * w) + (quaternion.y * z) - (quaternion.z * y),
		(quaternion.w * y) - (quaternion.x * z) + (quaternion.y * w) + (quaternion.z * x),
		(quaternion.w * z) + (quaternion.x * y) - (quaternion.y * x) + (quaternion.z * w),
		(quaternion.w * w) - (quaternion.x * x) - (quaternion.y * y) - (quaternion.z * z)
	};
}

Quaternion Quaternion::MultiplyRotationAxis(const Vector3& axis, float angle) const
{
	Vector3 normalized_axis{ axis.Normalize() };

	float sin{ sinf(angle * 0.5f) };

	return *this * Quaternion(normalized_axis.x * sin, normalized_axis.y * sin, normalized_axis.z * sin, cosf(angle * 0.5f));
}

Quaternion Quaternion::RotationAxis(const Vector3& axis, float angle)
{
	Vector3 normalized_axis{ axis.Normalize() };

	float sin{ sinf(angle * 0.5f) };

	return { normalized_axis.x * sin, normalized_axis.y * sin, normalized_axis.z * sin, cosf(angle * 0.5f) };
}

Quaternion Quaternion::RotationMatrix(const Matrix& rotation)
{
	Quaternion out{};

	// http://marupeke296.sakura.ne.jp/DXG_No58_RotQuaternionTrans.html

	// 最大成分を検索
	float elem[4]{}; // 0:x, 1:y, 2:z, 3:w
	elem[0] = rotation._11 - rotation._22 - rotation._33 + 1.0f;
	elem[1] = -rotation._11 + rotation._22 - rotation._33 + 1.0f;
	elem[3] = rotation._11 + rotation._22 + rotation._33 + 1.0f;
	elem[2] = -rotation._11 - rotation._22 + rotation._33 + 1.0f;

	unsigned biggestIndex = 0;
	for (int i = 1; i < 4; i++)
	{
		if (elem[i] > elem[biggestIndex])
		{
			biggestIndex = i;
		}
	}

	if (elem[biggestIndex] < 0.0f)
	{
		// 作成失敗
		out.x = 0;
		out.y = 0;
		out.z = 0;
		out.w = 1;
		return out;
	}

	// 最大要素の値を算出
	float* q[4]      = { &out.x, &out.y, &out.z, &out.w };
	float v          = sqrtf(elem[biggestIndex]) * 0.5f;
	*q[biggestIndex] = v;
	float mult       = 0.25f / v;

	switch (biggestIndex)
	{
		case 0: // x
			*q[1] = (rotation._12 + rotation._21) * mult;
			*q[2] = (rotation._31 + rotation._13) * mult;
			*q[3] = (rotation._23 - rotation._32) * mult;
			break;
		case 1: // y
			*q[0] = (rotation._12 + rotation._21) * mult;
			*q[2] = (rotation._23 + rotation._32) * mult;
			*q[3] = (rotation._31 - rotation._13) * mult;
			break;
		case 2: // z
			*q[0] = (rotation._31 + rotation._13) * mult;
			*q[1] = (rotation._23 + rotation._32) * mult;
			*q[3] = (rotation._12 - rotation._21) * mult;
			break;
		case 3: // w
			*q[0] = (rotation._23 - rotation._32) * mult;
			*q[1] = (rotation._31 - rotation._13) * mult;
			*q[2] = (rotation._12 - rotation._21) * mult;
			break;
	}

	return out;
}

Quaternion Quaternion::Normalize() const
{
	float length_inverse{ 1 / sqrtf(Dot(*this)) };
	//float length_inverse{ 1 / sqrtf(x * x + y + y * z * z + w * w) };
	return *this * length_inverse;
}

Quaternion Quaternion::Slerp(const Quaternion& quaternion, float interpolated) const
{
	// Result = Q0 * sin((1.0 - t) * Omega) / sin(Omega) + Q1 * sin(t * Omega) / sin(Omega)

	if (interpolated <= 0) { return *this; }
	if (interpolated >= 1) { return quaternion; }

	float dot = (x * quaternion.x + y * quaternion.y + z * quaternion.z + w * quaternion.w);
	float ss = 1.0f - (dot * dot);
	if (ss <= 0.0f) { return *this; }

	Quaternion target{ quaternion };

	if (dot < 0.0f)
	{
		target.x = -target.x;
		target.y = -target.y;
		target.z = -target.z;
		target.w = -target.w;
		dot = -dot;
	}

	if (dot > 1.0f) dot = 1.0f;

	float ph = acosf(dot);
	float s = 1.0f / sinf(ph);
	float s1 = sinf(ph * (1.0f - interpolated)) * s;
	float s2 = sinf(ph * interpolated) * s;

	return
	{
		x * s1 + target.x * s2,
		y * s1 + target.y * s2,
		z * s1 + target.z * s2,
		w * s1 + target.w * s2
	};

	//else
	//{
	//	Quaternion inverse_quaternion{ quaternion.Inverse() };

	//	return
	//	{
	//		x * sinf((1.0f - interpolated) * ph) / sinf(ph) + inverse_quaternion.x * sinf(interpolated * ph) / sinf(ph),
	//		y * sinf((1.0f - interpolated) * ph) / sinf(ph) + inverse_quaternion.y * sinf(interpolated * ph) / sinf(ph),
	//		z * sinf((1.0f - interpolated) * ph) / sinf(ph) + inverse_quaternion.z * sinf(interpolated * ph) / sinf(ph),
	//		w * sinf((1.0f - interpolated) * ph) / sinf(ph) + inverse_quaternion.w * sinf(interpolated * ph) / sinf(ph)
	//	};
	//}

}

std::tuple<Vector3, Vector3, Vector3> Quaternion::GetRotationComponents() const
{
	Matrix rotation_matrix{ Matrix::CreateRotation(*this) };
	return { Vector3(rotation_matrix.m[0]), Vector3(rotation_matrix.m[1]), Vector3(rotation_matrix.m[2]) };
}

float Quaternion::Dot(const Quaternion& quaternion) const
{
	return x * quaternion.x + y * quaternion.y + z * quaternion.z + w * quaternion.w;
}
