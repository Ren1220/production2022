#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>

#include "Arithmetic/Arithmetic.h"

/* --- 前方宣言 ---- */

class Actor;
class Collider;

/* --- 宣言 ---- */

/// <summary>
/// アクタの各コライダーとの衝突判定を行う
/// </summary>
class ColliderManager
{
public:

	void Update();

	/// <summary>
	/// 引数の球と衝突判定を行う
	/// </summary>
	/// <param name="sphere_position"></param>
	/// <param name="sphere_radius"></param>
	/// <param name="target_group_name">アクタのグループ</param>
	/// <returns>交差したアクタの参照</returns>
	std::vector<std::shared_ptr<Actor>> GetCollidedActorsWithSphere(
		const Vector3& sphere_position, float sphere_radius, const char* target_group_name = nullptr);

	void OnHitEventActors();

	/// <summary>
	/// Actorクラス内でコライダーを生成する際にこのクラスが弱参照を持つようにする。
	/// </summary>
	/// <param name="collider"></param>
	void RegisterCollider(const std::shared_ptr<Collider>& collider);

private:

	/// <summary>
	/// 参照先が既に解放されたコライダーをリストから除外する。
	/// </summary>
	void RemoveExpiredCollider();

	std::vector<std::weak_ptr<Collider>> w_colliders;
};
