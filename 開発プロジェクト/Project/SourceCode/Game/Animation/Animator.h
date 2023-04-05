#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <optional>

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "Animation.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

/// <summary>
/// アニメーションコンポーネント
/// </summary>
class Animator
{
public:

	Animator();
	~Animator() = default;

	void Start();
	void End();
	void Update(float elapsed_time);
	void ImGuiRender();

	/// <summary>
	/// 現在のアニメーション時間に応じた再生中のアニメーションのイベントを実行する
	/// </summary>
	void AnimationEvent();

	/// <summary>
	/// アニメションの登録
	/// </summary>
	/// <param name="key">設定するキー</param>
	/// <param name="filepath">登録するアニメーションのインデックス</param>
	const std::shared_ptr<Animation>& RegisterAnimation(uint32_t key, size_t animation_index);

	/// <summary>
	/// <para>登録したアニメーションをキーで呼び出し、再生する。</para>
	/// <para>再生するアニメーションに開始時コールバックイベントが登録されている場合実行させる。</para>
	/// <para>尚、前回再生中のアニメーションが存在し、その終了時コールバックイベントが登録されている場合、始めに実行させる</para>
	/// <para>第三引数で補間速度を設定することでアニメーションブレンドできるが、正直バグがありそうなので注意する（特にアニメーションイベントの兼ね合いが難しい）</para>
	/// </summary>
	/// <param name="key">再生するアニメーションのキー</param>
	/// <param name="is_loop">ループ再生を行うか</param>
	/// <param name="interpolation">ブレンド補間速度。FLT_MAXの時はブレンドしない</param>
	void PlayAnimation(uint32_t key, bool is_loop, float interpolation = FLT_MAX);

	/// <summary>
	/// <para>現在再生中のアニメーションを終了させる。					</para>
	/// <para>終了時コールバックイベントが登録されている場合実行させる。</para>
	/// </summary>
	void StopAnimation();

	bool IsPlayAnimation() const { return !has_played_animation; } // アニメーション再生中か

	bool HasPlayedAnimation() const { return has_played_animation; }

	std::shared_ptr<Animation> GetCurrentPlayAnimation() const;

	const std::shared_ptr<Animation>& GetAnimation(uint32_t key) const;

	size_t CountOfRegisteredAnimation() const { return registered_animations.size(); }

	void SetActor(const std::shared_ptr<Actor>& actor);
	std::shared_ptr<Actor> GetActor() const;

	float GetAnimBlendRatio() const { return blend_ratio; }
	void SetAnimBlendRatio(float f) { blend_ratio = f; }

private:

	/// <summary>
	/// 登録されているアニメーションのキーと一致するアニメーションを設定する
	/// </summary>
	/// <param name="key"></param>
	void SetCurrentAnimation(uint32_t key);

private:

	std::unordered_map<uint32_t, std::shared_ptr<Animation>> registered_animations;
	std::weak_ptr<Actor> w_actor;

	std::optional<std::unordered_map<uint32_t, std::shared_ptr<Animation>>::const_iterator> current_animation;
	std::optional<std::unordered_map<uint32_t, std::shared_ptr<Animation>>::const_iterator> next_animation;

	float interpolation;
	float blend_ratio;

	float animation_timer1;				// アニメーションタイマー
	float animation_timer2;				// アニメーションタイマー
	bool is_loop;						// ループ再生を行うか
	bool has_looped;					// ループ再生を行ったか
	bool has_played_animation;			// アニメーションを再生したか
};
