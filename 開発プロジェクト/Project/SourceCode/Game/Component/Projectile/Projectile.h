#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Component.h"

/* --- 宣言 ---- */

class Projectile : public Component
{
public:

	Projectile(const std::shared_ptr<Actor>& owner, float alive_time);
	~Projectile() override = default;

protected:

	/// <summary>
	/// 生存時間を経過させ、生存可能時間を超えた場合破棄
	/// </summary>
	/// <param name="elapsed_time"></param>
	void CheckAlived(float elapsed_time);

	/// <summary>
	/// 壁に衝突時破棄する
	/// </summary>
	void RemoveWhenCollideWall();

	/// <summary>
	/// 対象にダメージを与える
	/// </summary>
	virtual void DamageTo(
		const std::shared_ptr<Actor>& dst, 
		int32_t val);


	float alive_time; // 生存可能時間
	float timer;	  // 生存時間
	
	// この弾を生み出したアクタ
	std::weak_ptr<Actor> w_owner;
};
