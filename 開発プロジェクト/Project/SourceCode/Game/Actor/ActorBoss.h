#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Actor.h"

/* --- 宣言 ---- */

class ActorBoss : public Actor
{
public:

	enum AnimationKey : uint32_t
	{
		Idle,
		Dead,
		Damaged,
		RoaringBegin,
		Roaring,
		Run,
		Tackle,
		Jump,
		BackJump,
		JumpAttack,
		Walk,
		Attack0, // パンチ
		Attack1, // 振り被り
		Kick,    // 石を蹴とばす
		Return,  // 初期位置にジャンプ
	};


	ActorBoss()
		: Actor()
	{

	}

	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });

private:

	void RegisterAnimation();
};
