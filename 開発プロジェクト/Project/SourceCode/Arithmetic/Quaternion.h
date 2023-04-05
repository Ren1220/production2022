#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <format>

#include "Vector.h"
#include "Matrix.h"

/* --- 前方宣言 ---- */


/* --- 宣言 ---- */

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;


	Quaternion() = default;

	constexpr Quaternion(float x, float y, float z, float w)
		:x(x), y(y), z(z), w(w) {}

	constexpr Quaternion(const Quaternion& quaternion) = default;

	constexpr Quaternion(const float begin[])
		:Quaternion(begin[0],begin[1],begin[2],begin[3]) {}

	Quaternion(const Vector3& axis, float angle);

	Quaternion(const Matrix& rotation);

	~Quaternion() = default;


	/// <summary>
	/// 値を文字列にして返す
	/// </summary>
	/// <returns></returns>
	std::string GetString() const
	{
		return std::format("({:.4f}, {:.4f}, {:.4f}, {:.4f})", x, y, z, w);
	}

	/// <summary>
	/// 単位クォータニオン
	/// </summary>
	static constexpr Quaternion Identity()
	{
		return { 0, 0, 0, 1 };
	}

	/// <summary>
	/// 共役クォータニオン
	/// </summary>
	Quaternion Conjugate() const
	{
		return { -x, -y, -z, w };
	}

	/// <summary>
	/// 逆クォータニオン
	/// </summary>
	Quaternion Inverse() const;

	/// <summary>
	/// クォータニオンの乗算
	/// </summary>
	Quaternion Multiply(const Quaternion& quaternion) const;

	/// <summary>
	/// 任意軸回転クォータニオンとの乗算
	/// </summary>
	/// <param name="axis">回転軸ベクトル</param>
	/// <param name="angle">回転量</param>
	Quaternion MultiplyRotationAxis(const Vector3& axis, float angle) const;

	/// <summary>
	/// 任意軸回転クォータニオンの作成
	/// </summary>
	/// <param name="axis">回転軸ベクトル</param>
	/// <param name="angle">回転量</param>
	static Quaternion RotationAxis(const Vector3& axis, float angle);

	/// <summary>
	/// 回転行列をクォータニオンに変換する
	/// </summary>
	static Quaternion RotationMatrix(const Matrix& rotation);

	/// <summary>
	/// 正規化
	/// </summary>
	Quaternion Normalize() const;

	/// <summary>
	/// 球形補間
	/// </summary>
	/// <param name="quaternion"></param>
	/// <param name="interpolated">補間値</param>
	Quaternion Slerp(const Quaternion& quaternion, float interpolated) const;

	/// <summary>
	/// クォータニオンの各ベクトル成分の取得
	/// </summary>
	/// <returns>[ right, up, forward ]</returns>
	std::tuple<Vector3, Vector3, Vector3> GetRotationComponents() const;

	float Dot(const Quaternion& quaternion) const;

	/* --- 算術演算子 ---- */

	Quaternion operator+(const Quaternion& quaternion) const
	{
		return { x + quaternion.x, y + quaternion.y, z + quaternion.z, w + quaternion.w };
	}

	Quaternion operator-(const Quaternion& quaternion) const
	{
		return { x - quaternion.x, y - quaternion.y, z - quaternion.z, w - quaternion.w };
	}

	Quaternion operator*(const Quaternion& quaternion) const
	{
		return Multiply(quaternion);
	}

	Quaternion operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	Quaternion operator/(float divide) const
	{
		return	{ x / divide, y / divide, z / divide, w / divide };
	}

	void operator+=(const Quaternion& quaternion)
	{
		x += quaternion.x; y += quaternion.y; z += quaternion.z; w += quaternion.w;
	}

	void operator-=(const Quaternion& quaternion)
	{
		x -= quaternion.x; y -= quaternion.y; z -= quaternion.z; w -= quaternion.w;
	}

	void operator*=(const Quaternion& quaternion)
	{
		x = (quaternion.w * x) + (quaternion.x * w) + (quaternion.y * z) - (quaternion.z * y);
		y = (quaternion.w * y) - (quaternion.x * z) + (quaternion.y * w) + (quaternion.z * x);
		z = (quaternion.w * z) + (quaternion.x * y) - (quaternion.y * x) + (quaternion.z * w);
		w = (quaternion.w * w) - (quaternion.x * x) - (quaternion.y * y) - (quaternion.z * z);
	}

	void operator*=(float scalar)
	{
		x *= scalar; y *= scalar; z *= scalar; w *= scalar;
	}

	void operator/=(float divide)
	{
		x /= divide; y /= divide; z /= divide; w /= divide;
	}

};

