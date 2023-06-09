#pragma once

/* --- wb_t@CÌCN[h ---- */

#include "Collider.h"

/* --- é¾ ---- */

class ColliderCapsule : public Collider
{
public:

	ColliderCapsule(const char* name, const Vector3& start, const Vector3& length, float radius, const Vector4& color = { 1, 1, 1, 1 }, bool is_intersect_only = false)
		: Collider(name, start, color, is_intersect_only), length(length), radius(radius) {}
	~ColliderCapsule() override = default;

	/// <summary>
	/// Jn
	/// </summary>
	void Start() override;
	/// <summary>
	/// XV
	/// </summary>
	/// <param name="elapsed_time">oßÔ</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui`æ
	/// </summary>
	void ImGuiRender() override;

	void SetLength(const Vector3& l) { length = l; }

public:

	void SetRadius(float r) { radius = r; }

	float GetRadius() const { return radius; } // e²Ì¼a
	const Vector3& GetLength() const { return length; }

	bool IntersectSphere(const Vector3& sphere_position, float sphere_radius) const;

private:

	/// <summary>
	/// AABBÆÌð·»è
	/// </summary>
	bool IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration) override;
	/// <summary>
	/// ÆÌð·»è
	/// </summary>
	bool IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration) override;
	/// <summary>
	/// JvZÆÌð·»è
	/// </summary>
	bool IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration) override;

private:

	Vector3 length;
	float radius; // ¼a

};
