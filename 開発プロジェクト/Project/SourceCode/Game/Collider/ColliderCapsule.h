#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Collider.h"

/* --- 宣言 ---- */

class ColliderCapsule : public Collider
{
public:

	ColliderCapsule(const char* name, const Vector3& start, const Vector3& length, float radius, const Vector4& color = { 1, 1, 1, 1 }, bool is_intersect_only = false)
		: Collider(name, start, color, is_intersect_only), length(length), radius(radius) {}
	~ColliderCapsule() override = default;

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

	void SetLength(const Vector3& l) { length = l; }

public:

	void SetRadius(float r) { radius = r; }

	float GetRadius() const { return radius; } // 各軸の半径
	const Vector3& GetLength() const { return length; }

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
	/// <summary>
	/// カプセルとの交差判定
	/// </summary>
	bool IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration) override;

private:

	Vector3 length;
	float radius; // 半径

};
