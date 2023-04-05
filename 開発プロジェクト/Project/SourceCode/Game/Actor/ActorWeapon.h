#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Actor.h"

/* --- 宣言 ---- */

class ActorWeapon : public Actor
{
public:

	ActorWeapon()
		: Actor()
	{

	}

	/// <summary>
	/// 初期化。コンポーネント、初期パラメータを設定する。
	/// </summary>
	/// <param name="device"></param>
	/// <param name="name"></param>
	/// <param name="attack_power">武器攻撃力</param>
	/// <param name="durability">武器耐久値</param>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		int32_t attack_power,
		int32_t durability,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });
};
