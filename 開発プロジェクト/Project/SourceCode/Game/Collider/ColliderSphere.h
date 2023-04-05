#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Collider.h"

/* --- 宣言 ---- */

class ColliderSphere : public Collider
{
public:

	ColliderSphere(const char* name, const Vector3& center, float radius, const Vector4& color = {1, 1, 1, 1}, bool is_intersect_only = false)
		 : Collider(name, center, color, is_intersect_only), radius(radius) {}
	~ColliderSphere() override = default;

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
	/// 他のアクタのコライダーと交差したときに実行される関数
	/// </summary>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;

public:

	void SetRadius(float r) { radius = r; }
	float GetRadius() const { return radius; } // 半径

	bool IntersectSphere(const Vector3& sphere_position, float sphere_radius) const;

private:
	/// <summary>
	/// AABBとの交差判定
	/// </summary>
	bool IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration) override;
	/// <summary>
	/// 球との交差判定
	/// </summary>
	bool IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration) override;
	///// <summary>
	///// 垂直円柱との交差判定
	///// </summary>
	//bool IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration) override;
	/// <summary>
	/// カプセルとの交差判定
	/// </summary>
	bool IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration) override;

private:

	float radius; // 半径

};
