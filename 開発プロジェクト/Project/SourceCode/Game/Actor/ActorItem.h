#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Actor.h"

/* --- 宣言 ---- */

class ActorItem : public Actor
{
public:

	ActorItem()
		: Actor()
	{

	}

	/// <summary>
	/// 初期化。コンポーネント、初期パラメータを設定する。
	/// </summary>
	/// <param name="name"></param>
	/// <param name="group_name"></param>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	/// <param name="model_filepath"></param>
	/// <param name="is_static_mesh"></param>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });
};

