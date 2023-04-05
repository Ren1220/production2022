#include "ColliderVCylinder.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "ColliderCapsule.h"

/* --- 定義 ---- */

void ColliderVCylinder::Start()
{
}

void ColliderVCylinder::Update(float elapsed_time)
{
	Collider::Update(elapsed_time);

	// デバッグ用プリミティブの描画
	if (show_debug_primitive)
	{
		DebugPrimitive::GetInstance().DrawCylinder(standard_point + offset, Vector3(radius, half_length, radius), color);
	}
}

void ColliderVCylinder::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}

void ColliderVCylinder::ImGuiRender()
{
	Collider::ImGuiRender();
}

bool ColliderVCylinder::IntersectSphere(const Vector3& sphere_position, float sphere_radius) const
{
	// 円柱の高さの最大値が最小値を下回る場合は交差していない
	if (standard_point.y + standard_point.y + half_length < sphere_position.y - sphere_radius ||
		standard_point.y + standard_point.y - half_length > sphere_position.y + sphere_radius)
	{
		return false;
	}

	Vector3 near_p = sphere_position;

	// 円柱の範囲における球の中心までの最近点を求める
	near_p.x = std::clamp(near_p.x, standard_point.x + offset.x - radius, standard_point.x + offset.x + radius);
	near_p.z = std::clamp(near_p.z, standard_point.z + offset.z - radius, standard_point.z + offset.z + radius);
	near_p.y = std::clamp(near_p.y, standard_point.y + offset.y - half_length, standard_point.y + offset.y + half_length);

	// 球の中心と最近点の距離の平方が円柱と球の半径の和の平方を下回る場合交差している
	if ((sphere_position - near_p).LengthSquare() < (sphere_radius + radius) * (sphere_radius + radius))
	{
		return true;
	}

	return false;
}

bool ColliderVCylinder::IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	const Vector3& opponent_radius{ opponent->GetRadius() };

	// 円柱の高さの最大値が最小値を下回る場合は交差していない
	if (standard_point.y + standard_point.y + half_length < opponent_standard_point.y + opponent_offset.y - opponent_radius.y ||
		standard_point.y + standard_point.y - half_length > opponent_standard_point.y + opponent_offset.y + opponent_radius.y)
	{
		return false;
	}

	const Vector3 cylinder_position{ standard_point + offset };


	// AABBの範囲内における円柱の中心への最近点を求める
	Vector3 near_p{ cylinder_position };

	near_p = near_p.Clamp(opponent_standard_point + opponent_offset - opponent_radius,  // min
						  opponent_standard_point + opponent_offset + opponent_radius); // max

	// 最近点と球の中心の距離が球の半径より小さい場合交差している

	const float distance{ (cylinder_position - near_p).Length() };

	if (distance < radius)
	{
		out_penetration = radius - distance;

		return true;
	}

	return false;
}

bool ColliderVCylinder::IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_center{ opponent->GetOffset() };
	float opponent_radius{ opponent->GetRadius() };

	// 円柱の高さの最大値が最小値を下回る場合は交差していない
	if (standard_point.y + standard_point.y + half_length < opponent_standard_point.y + opponent_center.y - opponent_radius ||
		standard_point.y + standard_point.y - half_length > opponent_standard_point.y + opponent_center.y + opponent_radius)
	{
		return false;
	}

	const Vector3 sphere_position{ opponent_standard_point + opponent_center };
	Vector3 near_p = sphere_position;

	// 円柱の範囲における球の中心までの最近点を求める
	near_p.x = std::clamp(near_p.x, standard_point.x + offset.x - radius, standard_point.x + offset.x + radius);
	near_p.z = std::clamp(near_p.z, standard_point.z + offset.z - radius, standard_point.z + offset.z + radius);
	near_p.y = std::clamp(near_p.y, standard_point.y + offset.y - half_length, standard_point.y + offset.y + half_length);

	// 球の中心と最近点の距離が円柱と球の半径の和を下回る場合交差している
	const float distance{ (sphere_position - near_p).Length() };

	if ( distance < (opponent_radius + radius))
	{
		out_penetration = (opponent_radius + radius) - distance;
		return true;
	}

	return false;
}

bool ColliderVCylinder::IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	float opponent_radius{ opponent->GetRadius() };
	float opponent_half_length{ opponent->GetHalfLength() };

	// 円柱の高さの最大値が最小値を下回る場合は交差していない
	if (standard_point.y + offset.y + half_length < opponent_standard_point.y + opponent_offset.y - opponent_half_length ||
		standard_point.y + offset.y - half_length > opponent_standard_point.y + opponent_offset.y + opponent_half_length)
	{
		return false;
	}

	// 円柱の距離が円柱の半径の和より小さい場合交差している
	const float distance{ ((standard_point + offset) - (opponent_standard_point + opponent_offset)).Length() };

	if (distance < (radius + opponent_radius))
	{
		out_penetration = (radius + opponent_radius) - distance;
		return true;
	}

	return false;
}

bool ColliderVCylinder::IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration)
{
	return false;
}
