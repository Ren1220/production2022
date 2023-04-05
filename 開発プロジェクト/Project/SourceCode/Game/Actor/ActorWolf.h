#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Actor.h"

#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- 宣言 ---- */

class ActorWolf : public Actor
{
public:

	enum AnimationIndex : uint32_t
	{
		Run,
		Walk,
		CrouchWalk, // 屈み歩き
		Idle,
		Sit,
		Appearance, // 出現
		Attack,
		Attack2,
		Damaged,
		Dead,
		Howling,
	};

	ActorWolf()
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
		const Vector3& scale = { 2, 2, 2 },
		bool has_summoned = false);

	void PlayAppearanceEffect();
	void DestroyAppearanceEffect();

private:

	void RegisterAnimation();

	EffekseerHandle appearance_effect;
};
