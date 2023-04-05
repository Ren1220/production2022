#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <array>

#include "../Component.h"

#include "HealthEffect.h"


/* --- 宣言 ---- */


class Health : public Component
{
public:

	Health(int32_t current_health, int32_t max_health);
	~Health() override;

	void Start() override;
	void End() override;
	void Update(float elapsed_time) override;
	void ImGuiRender() override;

public:

	/// <summary>
	/// ダメージを与える
	/// </summary>
	/// <param name="attacker">攻撃してきた相手</param>
	/// <param name="damage">与えるダメージ量</param>
	/// <param name="invincible_time">設定する無敵時間</param>
	/// <param name="camera_shake_time">カメラシェイクを行う時間</param>
	/// <returns>ダメージを与えられたか</returns>
	bool Damaged(
		const std::shared_ptr<Actor>& attacker,
		int32_t damage, 
		float invincible_time, 
		float camera_shake_time);

	/// <summary>
	/// 回復させる最大体力を超えて回復はしない
	/// </summary>
	/// <param name="value">回復量</param>
	void Recover(int32_t value);

	/// <summary>
	/// 無敵時間を設定する
	/// </summary>
	void SetInvincibleTimer(float t) { invincible_timer = t; }

	int32_t GetHealth() const { return health; }
	int32_t GetMaxHealth() const { return max_health; }
	float GetInvincibleTimer() const { return invincible_timer; }

	/// <summary>
	/// このフレームに攻撃をしてきた相手（攻撃の成功の可否は問わない）
	/// </summary>
	const std::vector<std::weak_ptr<Actor>>& GetAttackers() const { return w_attackers; }

	bool IsInvincible() const { return invincible_timer > 0.0f; }

	/// <summary>
	/// 状態異常によるダメージは含まれない
	/// </summary>
	bool IsDamaged() const { return is_damaged; }

	bool IsDead() const { return is_dead; }

	/// <summary>
	/// ダメージフラグをリセットする
	/// このフレームに受けたダメージの総量も
	/// </summary>
	void ResetDamagedFlag();

	/// <summary>
	/// 状態とその効果時間を設定する
	/// </summary>
	void SetEffect(HealthEffectID id, float time);

private:

	/// <summary>
	/// 各状態に応じた効果を適用する
	/// </summary>
	void ApplyEffect();

	int32_t damage; // このフレームに受けたダメージの合計値
	int32_t health;
	int32_t max_health;

	// 0を超える値の場合無敵状態として扱われる
	float invincible_timer;

	bool is_dead;
	bool is_damaged; // 状態異常によるダメージは含まれない

	// エフェクトの効果を指定秒毎に実行する為のタイマー
	float apply_effect_timer;

	// 受けた状態に応じた現在再生中のエフェクト
	std::array<std::unique_ptr<HealthEffect>, static_cast<size_t>(HealthEffectID::ElementCount)> health_effects;

	// このフレームに攻撃をしてきた相手（攻撃の成功の可否は問わない）
	std::vector<std::weak_ptr<Actor>> w_attackers;
};

