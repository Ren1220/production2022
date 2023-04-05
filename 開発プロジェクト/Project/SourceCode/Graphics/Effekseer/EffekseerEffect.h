#pragma once

/* --- 追加のヘッダファイルのインクルード ---- */

#include <Effekseer.h>
#include <optional>

#include "Arithmetic/Arithmetic.h"

/* --- 宣言 ---- */

class EffekseerEffect
{
public:

	EffekseerEffect() = default;
	~EffekseerEffect()
	{
		// 破棄処理
		if (effect != nullptr)
		{
			effect->Release();
			effect = nullptr;
		}
	}

	void Create(Effekseer::Manager* manager, const char* filepath);

	/// <summary>
	/// このエフェクトを新しく生成して、生成したエフェクトを操作するためのハンドルを返す。
	/// </summary>
	Effekseer::Handle Play(const Vector3& position);

private:

	Effekseer::Effect* effect{ nullptr };
};