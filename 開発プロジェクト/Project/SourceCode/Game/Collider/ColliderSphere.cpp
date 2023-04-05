#include "ColliderSphere.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "IntersectUser/Intersect.h"

#include "ColliderAABB.h"
//#include "ColliderVCylinder.h"
#include "ColliderCapsule.h"


/* --- 定義 ---- */

void ColliderSphere::Start()
{
}

void ColliderSphere::Update(float elapsed_time)
{
	Collider::Update(elapsed_time);

	// デバッグ用プリミティブの描画
	if (show_debug_primitive && is_active)
	{
		DebugPrimitive::GetInstance().DrawSphere(standard_point + offset, radius, color);
	}
}

void ColliderSphere::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}

void ColliderSphere::ImGuiRender()
{
	Collider::ImGuiRender();
}

bool ColliderSphere::IntersectSphere(const Vector3& sphere_position, float sphere_radius) const
{
	// 球の距離の平方が球の半径の和の平方より小さい場合交差している
	if ((sphere_position - standard_point + offset).LengthSquare() < (radius + sphere_radius) * (radius + sphere_radius))
	{
		return true;
	}

	return false;
}

bool ColliderSphere::IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	const Vector3& opponent_radius{ opponent->GetRadius() };

	const Vector3 sphere_position{ standard_point + offset };

	// AABBの範囲内における球の中心への最近点を求める
	Vector3 near_p{ sphere_position };

	near_p = near_p.Clamp(opponent_standard_point + opponent_offset - opponent_radius,  // min
						  opponent_standard_point + opponent_offset + opponent_radius); // max


	// 最近点と球の中心の距離が球の半径より小さい場合交差している
	const float distance{ (sphere_position - near_p).Length() };

	if (distance < radius)
	{
		out_penetration = radius - distance;
		return true;
	}

	return false;
}

bool ColliderSphere::IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	float opponent_radius{ opponent->GetRadius() };

	// 球の距離が球の半径の和より小さい場合交差している
	const float distance{ (opponent_standard_point + opponent_offset - standard_point + offset).Length() };

	if (distance < (radius + opponent_radius))
	{
		out_penetration = (radius + opponent_radius) - distance;
		return true;
	}

	return false;
}

//bool ColliderSphere::IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration)
//{
//	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
//	const Vector3& opponent_offset{ opponent->GetOffset() };
//	float opponent_radius{ opponent->GetRadius() };
//	float opponent_half_length{ opponent->GetHalfLength() };
//
//	// 円柱の高さの最大値が最小値を下回る場合は交差していない
//	if (opponent_standard_point.y + opponent_offset.y + opponent_half_length < standard_point.y + offset.y - radius ||
//		opponent_standard_point.y + opponent_offset.y - opponent_half_length > standard_point.y + offset.y + radius)
//	{
//		return false;
//	}
//
//	const Vector3 sphere_position{ standard_point + offset};
//	Vector3 near_p = sphere_position;
//
//	// 円柱の範囲における球の中心までの最近点を求める
//	near_p.x = std::clamp(near_p.x, opponent_standard_point.x + opponent_offset.x - opponent_radius,
//						  opponent_standard_point.x + opponent_offset.x + opponent_radius);
//	near_p.z = std::clamp(near_p.z, opponent_standard_point.z + opponent_offset.z - opponent_radius,
//						  opponent_standard_point.z + opponent_offset.z + opponent_radius);
//	near_p.y = std::clamp(near_p.y, opponent_standard_point.y + opponent_offset.y - opponent_half_length,
//						  opponent_standard_point.y + opponent_offset.y + opponent_half_length);
//
//	// 球の中心と最近点の距離が円柱と球の半径の和を下回る場合交差している
//	const float distance{ (sphere_position - near_p).Length() };
//
//	if (distance < (opponent_radius + radius))
//	{
//		out_penetration = (opponent_radius + radius) - distance;
//		return true;
//	}
//
//	return false;
//}

bool ColliderSphere::IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration)
{
	float dist;

	if (IntersectUser::IntersectSphereVSCapsule(
		standard_point,
		radius,
		opponent->GetStandardPoint(),
		opponent->GetStandardPoint()+ opponent->GetLength(),
		opponent->GetRadius(),
		dist))
	{
		out_penetration = radius + opponent->GetRadius() - dist;
		return true;
	}

	return false;
}
