#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <Effekseer.h>

#include "Arithmetic/Arithmetic.h"

#include "EffekseerManager.h"

/* --- 宣言 ---- */

/// <summary>
/// Effekseerのエフェクトを再生すると返されるハンドル。
/// このハンドルを使って生成したエフェクトを操作する。
/// </summary>
class EffekseerHandle
{
public:

	EffekseerHandle() = default;
	/// <summary>
	/// エフェクトのパスは"Data/Effect/"に続くファイルパス
	/// </summary>
	EffekseerHandle(
		const char* filepath, 
		const Vector3& position);

	~EffekseerHandle();

	void Initialize(
		const char* filepath,
		const Vector3& position);

	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale);
	void SetTransform(const Matrix& transform);

	void SetPosition(const Vector3& position);
	void SetRotation(const Vector3& axis, float angle);
	void SetRotation(const Quaternion& rotation);

	/// <summary>
	/// オイラー回転
	/// </summary>
	void SetRotation(const Vector3& radian);
	void SetScale(const Vector3& scale);
	void SetColor(const Vector4& color);

	/// <summary>
	/// エフェクトを破棄する。
	/// </summary>
	void Destroy();
	/// <summary>
	/// 表示非表示
	/// </summary>
	void SetShown(bool is_shown);

	/// <summary>
	/// ハンドルのエフェクトのインスタンスが存在しているか
	/// </summary>
	bool IsExist();

	void ImGuiRender();
private:

	Effekseer::Handle handle;

	Vector3 position;
	Vector3 angle;
	Vector3 scale;
};
