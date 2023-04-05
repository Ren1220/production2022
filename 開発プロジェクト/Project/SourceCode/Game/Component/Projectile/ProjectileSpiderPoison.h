#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Projectile.h"

/* --- 宣言 ---- */

class ProjectileSpiderPoison : public Projectile
{
public:

	ProjectileSpiderPoison(
		const std::shared_ptr<Actor>& owner, 
		int32_t power,
		const Vector3& direction,
		float speed = 1.0f,
		float max_speed = 2.0f,
		float alive_time = 100.0f);
	~ProjectileSpiderPoison() override = default;

	/// <summary>
	/// 開始処理
	/// </summary>
	void Start() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;
	/// <summary>
	/// 交差時に実行する関数
	/// </summary>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;

private:

	/// <summary>
	/// 対象にダメージを与える
	/// </summary>
	void DamageTo(
		const std::shared_ptr<Actor>& dst,
		int32_t val) override;


	int32_t power;
	Vector3 direction;
	float speed;
	float max_speed;

	
};
