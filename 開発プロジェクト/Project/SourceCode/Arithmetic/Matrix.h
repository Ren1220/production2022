#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <format>

#include "Vector.h"

/* --- 前方宣言 ---- */

struct Quaternion;

/* --- 宣言 ---- */

struct Matrix
{
	union
	{
		struct
		{
			float _11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44;
		};

		float m[4][4];
	};

	Matrix() = default;

	constexpr Matrix(float _11, float _12, float _13, float _14,
	                 float _21, float _22, float _23, float _24,
	                 float _31, float _32, float _33, float _34,
	                 float _41, float _42, float _43, float _44)
		: _11(_11), _12(_12), _13(_13), _14(_14),
		  _21(_21), _22(_22), _23(_23), _24(_24),
		  _31(_31), _32(_32), _33(_33), _34(_34),
		  _41(_41), _42(_42), _43(_43), _44(_44) {}

	Matrix(const Matrix& matrix)
		: Matrix(
			matrix._11, matrix._12, matrix._13, matrix._14, 
			matrix._21, matrix._22, matrix._23, matrix._24,
			matrix._31, matrix._32, matrix._33, matrix._34,
			matrix._41, matrix._42, matrix._43, matrix._44) {}

	~Matrix() = default;

	/// <summary>
	/// 値を文字列にして返す
	/// </summary>
	/// <returns></returns>
	std::string GetString() const
	{
		return std::format(
		                   "| {:.4f}, {:.4f}, {:.4f}, {:.4f} |\n| {:.4f}, {:.4f}, {:.4f}, {:.4f} |\n| {:.4f}, {:.4f}, {:.4f}, {:.4f} |\n| {:.4f}, {:.4f}, {:.4f}, {:.4f} |",
		                   _11, _12, _13, _14,
		                   _21, _22, _23, _24,
		                   _31, _32, _33, _34,
		                   _41, _42, _43, _44);
	}

	/// <summary>
	/// 単位行列の取得
	/// </summary>
	/// <returns></returns>
	static constexpr Matrix Identity()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// <summary>
	/// 右手座標系空間
	/// </summary>
	/// <returns></returns>
	static constexpr Matrix RhsYup()
	{
		return
		{
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	/// <summary>
	/// 右手系ビュー行列の作成
	/// </summary>
	/// <param name="eye">カメラ座標</param>
	/// <param name="target">カメラ注視点</param>
	/// <param name="up">カメラの上方向</param>
	static Matrix CreateLookAtRH(const Vector3& eye, const Vector3& target, const Vector3& up);

	/// <summary>
	/// 左手系ビュー行列の作成
	/// </summary>
	/// <param name="eye">カメラ座標</param>
	/// <param name="target">カメラ注視点</param>
	/// <param name="up">カメラの上方向</param>
	static Matrix CreateLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up);

	/// <summary>
	/// プロジェクション行列の作成
	/// </summary>
	static Matrix CreateProjection(float view_angle, float screen_width, float screen_height, float near_z, float far_z);

	/// <summary>
	/// プロジェクション行列の作成
	/// </summary>
	static Matrix CreateProjection(float view_angle, float aspect, float near_z, float far_z);

	/// <summary>
	/// 正射影行列の作成
	/// </summary>
	static Matrix CreateOrthographic(float screen_width, float screen_height, float near_z, float far_z);

	/// <summary>
	/// モデル変換行列の作成
	/// </summary>
	static Matrix CreateTransform(const Vector3& translation, const Quaternion& rotation, const Vector3& scaling);

	/// <summary>
	/// モデル変換行列の作成
	/// </summary>
	static Matrix CreateTransform(const Vector3& translation, const Vector3& rotation, const Vector3& scaling);

	/// <summary>
	/// 平行移動行列の作成
	/// </summary>
	static Matrix CreateTranslation(const Vector3& translation);

	/// <summary>
	/// クォータニオン回転行列の作成
	/// </summary>
	static Matrix CreateRotation(const Quaternion& rotation);

	/// <summary>
	/// オイラー回転行列の作成
	/// </summary>
	static Matrix CreateRotation(const Vector3& rotation);

	/// <summary>
	/// X軸回転行列の作成
	/// </summary>
	static Matrix CreateRotationX(float rotation_x);

	/// <summary>
	/// Y軸回転行列の作成
	/// </summary>
	static Matrix CreateRotationY(float rotation_y);

	/// <summary>
	/// Z軸回転行列の作成
	/// </summary>
	static Matrix CreateRotationZ(float rotation_z);

	/// <summary>
	/// スケーリング行列の作成
	/// </summary>
	static Matrix CreateScaling(const Vector3& scaling);


	/// <summary>
	/// 逆行列を求める
	/// </summary>
	Matrix Inverse() const;

	/// <summary>
	/// 転置行列を求める
	/// </summary>
	Matrix Transposition() const
	{
		return
		{
			_11, _21, _31, _41,
			_12, _22, _32, _42,
			_13, _23, _33, _43,
			_14, _24, _34, _44
		};
	}

	/// <summary>
	/// モデル空間行列を分解して引数の値に代入する
	/// </summary>
	void DecomposeTransform(Vector3& out_translation, Quaternion& out_rotation, Vector3& out_scaling) const;

	/// <summary>
	/// モデル空間行列を分解して引数の値に代入する
	/// </summary>
	void DecomposeTransform(Vector3& out_translation, Matrix& out_rotation, Vector3& out_scaling) const;

	/// <summary>
	/// モデル空間行列を分解して引数の値に代入する
	/// </summary>
	std::tuple<Vector3, Quaternion, Vector3> DecomposeTransform() const;

	/* --- 演算子 ---- */

	float& operator()(size_t row, size_t column)
	{
		return m[row][column];
	}

	Matrix operator*(const Matrix& matrix) const
	{
		return {
			// 左辺１行目と右辺の各列の内積値
			_11 * matrix._11 + _12 * matrix._21 + _13 * matrix._31 + _14 * matrix._41,
			_11 * matrix._12 + _12 * matrix._22 + _13 * matrix._32 + _14 * matrix._42,
			_11 * matrix._13 + _12 * matrix._23 + _13 * matrix._33 + _14 * matrix._43,
			_11 * matrix._14 + _12 * matrix._24 + _13 * matrix._34 + _14 * matrix._44,
			// 左辺２行目と右辺の各列の内積値
			_21 * matrix._11 + _22 * matrix._21 + _23 * matrix._31 + _24 * matrix._41,
			_21 * matrix._12 + _22 * matrix._22 + _23 * matrix._32 + _24 * matrix._42,
			_21 * matrix._13 + _22 * matrix._23 + _23 * matrix._33 + _24 * matrix._43,
			_21 * matrix._14 + _22 * matrix._24 + _23 * matrix._34 + _24 * matrix._44,
			// 左辺３行目と右辺の各列の内積値
			_31 * matrix._11 + _32 * matrix._21 + _33 * matrix._31 + _34 * matrix._41,
			_31 * matrix._12 + _32 * matrix._22 + _33 * matrix._32 + _34 * matrix._42,
			_31 * matrix._13 + _32 * matrix._23 + _33 * matrix._33 + _34 * matrix._43,
			_31 * matrix._14 + _32 * matrix._24 + _33 * matrix._34 + _34 * matrix._44,
			// 左辺４行目と右辺の各列の内積値
			_41 * matrix._11 + _42 * matrix._21 + _43 * matrix._31 + _44 * matrix._41,
			_41 * matrix._12 + _42 * matrix._22 + _43 * matrix._32 + _44 * matrix._42,
			_41 * matrix._13 + _42 * matrix._23 + _43 * matrix._33 + _44 * matrix._43,
			_41 * matrix._14 + _42 * matrix._24 + _43 * matrix._34 + _44 * matrix._44
		};
	}

	Matrix operator*(float scalar) const
	{
		// 各成分をスケール倍する
		return {
			_11 * scalar, _12 * scalar, _13 * scalar, _14 * scalar,
			_21 * scalar, _22 * scalar, _23 * scalar, _24 * scalar,
			_31 * scalar, _32 * scalar, _33 * scalar, _34 * scalar,
			_41 * scalar, _42 * scalar, _43 * scalar, _44 * scalar,
		};
	}
};
