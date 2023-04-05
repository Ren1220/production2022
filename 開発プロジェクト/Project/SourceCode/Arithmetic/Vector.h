#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <format>

/* --- 前方宣言 ---- */

struct Vector4;
struct Matrix;

/* --- 宣言 ---- */

/* ---------------- */
/* --- Vector3 ---- */
/* ---------------- */

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3() = default;

	constexpr Vector3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	constexpr Vector3(const Vector3& vector3)
		: Vector3(vector3.x, vector3.y, vector3.z){}

	constexpr Vector3(const float begin[])
		: Vector3(begin[0], begin[1], begin[2]) {}


	/// <summary>
	/// ゼロベクトルの取得
	/// </summary>
	/// <returns></returns>
	static constexpr Vector3 ZeroVector()
	{
		constexpr Vector3 zero_vector{ Vector3(0, 0, 0) };
		return zero_vector;
	}

	/// <summary>
	/// 値を文字列にして返す
	/// </summary>
	/// <returns></returns>
	std::string GetString() const
	{
		return std::format("({:.4f}, {:.4f}, {:.4f})", x, y, z);
	}

	/// <summary>
	/// 内積
	/// </summary>
	float Dot(const Vector3& vector3) const;

	/// <summary>
	/// 内積
	/// </summary>
	float Dot(const Vector4& vector4) const;

	/// <summary>
	/// 外積ベクトル
	/// </summary>
	Vector3 Cross(const Vector3& rhs) const;

	/// <summary>
	/// ベクトルの長さの平方
	/// </summary>
	float LengthSquare() const;

	/// <summary>
	/// ベクトルの長さ
	/// </summary>
	float Length() const;

	/// <summary>
	/// 引数のベクトルとの距離
	/// </summary>
	float Distance(const Vector3& vector3) const;

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <returns></returns>
	Vector3 Normalize() const;

	/// <summary>
	/// 引数ベクトル +- 誤差の範囲のベクトルか
	/// </summary>
	/// <param name="vector3"></param>
	/// <param name="rounding">+-誤差</param>
	bool NearEqual(const Vector3& vector3, const Vector3& rounding = { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON }) const;

	/// <summary>
	/// ゼロベクトルか
	/// </summary>
	/// <returns></returns>
	bool IsZero() const;

	/// <summary>
	/// 変換行列
	/// </summary>
	Vector3 Transform(const Matrix& transform_matrix) const; 

	/// <summary>
	/// 変換行列 w = 1に投影
	/// </summary>
	Vector3 TransformCoordinate(const Matrix& transform_matrix) const;

	/// <summary>
	/// 変換行列 4行目を計算に含めない
	/// </summary>
	Vector3 TransformNormal(const Matrix& transform_matrix) const;

	/// <summary>
	/// 線形補間
	/// </summary>
	Vector3 Lerp(const Vector3& vector3, float interpolated) const;

	/// <summary>
	/// ベクトルの成分を引数の範囲に収める
	/// </summary>
	/// <param name="min">各成分の最小値</param>
	/// <param name="max">各成分の最大値</param>
	Vector3 Clamp(const Vector3& min, const Vector3& max) const;

	/* --- 演算子 ---- */

	Vector3 operator+() const
	{
		return *this;
	}

	Vector3 operator-() const
	{
		return { -x, -y, -z };
	}

	Vector3 operator+(const Vector3& vector3) const
	{
		return { x + vector3.x, y + vector3.y, z + vector3.z };
	}

	Vector3 operator-(const Vector3& vector3) const
	{
		return { x - vector3.x, y - vector3.y, z - vector3.z };
	}

	Vector3 operator*(const Vector3& vector3) const
	{
		return { x * vector3.x, y * vector3.y, z * vector3.z };
	}

	Vector3 operator/(const Vector3& vector3) const
	{
		return { x / vector3.x, y / vector3.y, z / vector3.z };
	}

	Vector3& operator+=(const Vector3& vector3)
	{
		x += vector3.x;	y += vector3.y;	z += vector3.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& vector3)
	{
		x -= vector3.x;	y -= vector3.y;	z -= vector3.z;
		return *this;
	}

	Vector3& operator*=(const Vector3& vector3)
	{
		x *= vector3.x;	y *= vector3.y;	z *= vector3.z;
		return *this;
	}

	Vector3& operator/=(const Vector3& vector3)
	{
		x /= vector3.x;	y /= vector3.y;	z /= vector3.z;
		return *this;
	}

	Vector3 operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(float divide) const
	{
		return { x / divide, y / divide, z / divide };
	}

	Vector3& operator*=(float scalar)
	{
		x *= scalar;	y *= scalar;	z *= scalar;
		return *this;
	}

	Vector3& operator/=(float divide)
	{
		x /= divide; y /= divide; z /= divide;
		return *this;
	}
};

/* --- 外部演算子 ---- */

inline Vector3 operator*(float scalar, const Vector3& vector3)
{
	return { scalar * vector3.x, scalar * vector3.y, scalar * vector3.z };
}

/* ---------------- */
/* --- Vector2 ---- */
/* ---------------- */

struct Vector2
{
	float x;
	float y;

	Vector2() = default;

	constexpr Vector2(float x, float y)
		: x(x), y(y) {}

	constexpr Vector2(const float begin[])
		: Vector2(begin[0], begin[1]) {}


	/// <summary>
	/// 値を文字列にして返す
	/// </summary>
	/// <returns></returns>
	std::string GetString() const
	{
		return std::format("({:.4f}, {:.4f})", x, y);
	}

	/* --- 演算子 ---- */

	Vector2 operator+() const
	{
		return *this;
	}

	Vector2 operator-() const
	{
		return { -x, -y };
	}

	Vector2 operator+(const Vector2& vector4) const
	{
		return { x + vector4.x, y + vector4.y };
	}

	Vector2 operator-(const Vector2& vector4) const
	{
		return { x - vector4.x, y - vector4.y };
	}

	Vector2 operator*(const Vector2& vector4) const
	{
		return { x * vector4.x, y * vector4.y };
	}

	Vector2 operator/(const Vector2& vector4) const
	{
		return { x / vector4.x, y / vector4.y };
	}

	Vector2& operator+=(const Vector2& vector4)
	{
		x += vector4.x;	y += vector4.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& vector4)
	{
		x -= vector4.x;	y -= vector4.y;
		return *this;
	}

	Vector2& operator*=(const Vector2& vector4)
	{
		x *= vector4.x;	y *= vector4.y;
		return *this;
	}

	Vector2& operator/=(const Vector2& vector4)
	{
		x /= vector4.x;	y /= vector4.y;
		return *this;
	}

	Vector2 operator*(float scalar) const
	{
		return { x * scalar, y * scalar };
	}

	Vector2 operator/(float divide) const
	{
		return { x / divide, y / divide };
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;	y *= scalar;
		return *this;
	}

	Vector2& operator/=(float divide)
	{
		x /= divide; y /= divide;
		return *this;
	}
};

/* --- 外部演算子 ---- */

inline Vector2 operator*(float scalar, const Vector2& vector4)
{
	return { scalar * vector4.x, scalar * vector4.y };
}



/* ---------------- */
/* --- Vector4 ---- */
/* ---------------- */

struct Vector4
{
	float x;
	float y;
	float z;
	float w;

	Vector4() = default;

	constexpr Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{
	}

	constexpr Vector4(const Vector3& vector3, float w)
		: x(vector3.x), y(vector3.y), z(vector3.z), w(w)
	{
		
	}

	constexpr Vector4(const Vector4& vector4)
		: Vector4(vector4.x, vector4.y, vector4.z, vector4.w)
	{
	}

	constexpr Vector4(const float begin[])
		: Vector4(begin[0], begin[1], begin[2], begin[3]) {}


	/// <summary>
	/// 値を文字列にして返す
	/// </summary>
	/// <returns></returns>
	std::string GetString() const
	{
		return std::format("({:.4f}, {:.4f}, {:.4f}, {:.4f})", x, y, z, w);
	}

	/* --- 算術関数 ---- */


	/// <summary>
	/// 引数ベクトル +- 誤差の範囲のベクトルか
	/// </summary>
	/// <param name="vector4"></param>
	/// <param name="rounding">+-誤差</param>
	bool NearEqual(const Vector4& vector4, const Vector4& rounding = { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON }) const
	{
		_ASSERT_EXPR(rounding.x > 0 && rounding.y > 0 && rounding.z > 0 && rounding.w > 0, L"誤差の範囲の値が不正です");
		return abs(x - vector4.x) < rounding.x && abs(y - vector4.y) < rounding.y && abs(z - vector4.z) < rounding.z && abs(w - vector4.w) < rounding.w;
	}

	/* --- 演算子 ---- */

	Vector4 operator+() const
	{
		return *this;
	}

	Vector4 operator-() const
	{
		return { -x, -y, -z, -w };
	}

	Vector4 operator+(const Vector4& vector4) const
	{
		return { x + vector4.x, y + vector4.y, z + vector4.z ,w + vector4.w };
	}

	Vector4 operator-(const Vector4& vector4) const
	{
		return { x - vector4.x, y - vector4.y, z - vector4.z, w - vector4.w };
	}

	Vector4 operator*(const Vector4& vector4) const
	{
		return { x * vector4.x, y * vector4.y, z * vector4.z, w * vector4.w };
	}

	Vector4 operator/(const Vector4& vector4) const
	{
		return { x / vector4.x, y / vector4.y, z / vector4.z, w / vector4.w };
	}

	Vector4& operator+=(const Vector4& vector4)
	{
		x += vector4.x;	y += vector4.y;	z += vector4.z;	w += vector4.w;
		return *this;
	}

	Vector4& operator-=(const Vector4& vector4)
	{
		x -= vector4.x;	y -= vector4.y;	z -= vector4.z;	w -= vector4.w;
		return *this;
	}

	Vector4& operator*=(const Vector4& vector4)
	{
		x *= vector4.x;	y *= vector4.y;	z *= vector4.z; w *= vector4.w;
		return *this;
	}

	Vector4& operator/=(const Vector4& vector4)
	{
		x /= vector4.x;	y /= vector4.y;	z /= vector4.z; w /= vector4.w;
		return *this;
	}

	Vector4 operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	Vector4 operator/(float divide) const
	{
		return { x / divide, y / divide, z / divide, w / divide };
	}

	Vector4& operator*=(float scalar)
	{
		x *= scalar;	y *= scalar;	z *= scalar;	w *= scalar;
		return *this;
	}

	Vector4& operator/=(float divide)
	{
		x /= divide; y /= divide; z /= divide; w /= divide;
		return *this;
	}
};

/* --- 外部演算子 ---- */

inline Vector4 operator*(float scalar, const Vector4& vector4)
{
	return { scalar * vector4.x, scalar * vector4.y, scalar * vector4.z, scalar * vector4.w };
}

