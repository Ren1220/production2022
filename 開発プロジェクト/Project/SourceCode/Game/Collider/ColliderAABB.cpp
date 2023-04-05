#include "ColliderAABB.h"

/* --- 追加のヘッダファイルのインクルード ---- */

//#include "ColliderVCylinder.h"
#include "ColliderSphere.h"
#include "ColliderCapsule.h"

/* --- 定義 ---- */

void ColliderAABB::Start()
{
}

void ColliderAABB::Update(float elapsed_time)
{
	Collider::Update(elapsed_time);

	// デバッグ用プリミティブの描画
	if (show_debug_primitive && is_active)
	{
		DebugPrimitive::GetInstance().DrawCube(standard_point + offset, radius, color);
	}
}

void ColliderAABB::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}

void ColliderAABB::ImGuiRender()
{
	Collider::ImGuiRender();
}

bool ColliderAABB::IntersectSphere(const Vector3& sphere_position, float sphere_radius) const
{
	// AABBの範囲内における球の中心への最近点を求める
	Vector3 near_p{ sphere_position };

	near_p = near_p.Clamp(standard_point + offset - radius, standard_point + offset + radius);

	// 最近点と球の中心の距離の平方が球の半径の平方より小さい場合交差している
	if ((sphere_position - near_p).LengthSquare() < sphere_radius * sphere_radius)
	{
		return true;
	}

	return false;
}

bool ColliderAABB::IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	const Vector3& opponent_radius{ opponent->GetRadius() };

	// 各軸において交差しているか判定し、全ての軸で交差している場合交差している
	if (
		standard_point.x + offset.x - radius.x <= opponent_standard_point.x + opponent_offset.x + opponent_radius.x &&
		standard_point.x + offset.x + radius.x >= opponent_standard_point.x + opponent_offset.x - opponent_radius.x &&
		standard_point.y + offset.y - radius.y <= opponent_standard_point.y + opponent_offset.y + opponent_radius.y &&
		standard_point.y + offset.y + radius.y >= opponent_standard_point.y + opponent_offset.y - opponent_radius.y &&
		standard_point.z + offset.z - radius.z <= opponent_standard_point.z + opponent_offset.z + opponent_radius.z &&
		standard_point.z + offset.z + radius.z >= opponent_standard_point.z + opponent_offset.z - opponent_radius.z)
	{
		// TODO さぼり
		out_penetration = 0.05f;
		return true;
	}

	return false;
}

bool ColliderAABB::IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration)
{
	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
	const Vector3& opponent_offset{ opponent->GetOffset() };
	float opponent_radius{ opponent->GetRadius() };

	const Vector3 sphere_position{ opponent_standard_point + opponent_offset };

	// AABBの範囲内における球の中心への最近点を求める
	Vector3 near_p{ sphere_position };

	near_p = near_p.Clamp(standard_point + offset - radius, standard_point + offset + radius);

	// 最近点と球の中心の距離が球の半径より小さい場合交差している
	const float distance{ (sphere_position - near_p).Length() };
	if (distance < opponent_radius)
	{
		out_penetration = opponent_radius - distance;
		return true;
	}

	return false;
}

//bool ColliderAABB::IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration)
//{
//	const Vector3& opponent_standard_point{ opponent->GetStandardPoint() };
//	const Vector3& opponent_offset{ opponent->GetOffset() };
//	float opponent_radius{ opponent->GetRadius() };
//	float opponent_half_length{ opponent->GetHalfLength() };
//
//	// 円柱の高さの最大値が最小値を下回る場合は交差していない
//	if (opponent_standard_point.y + opponent_offset.y + opponent_half_length < standard_point.y + offset.y - radius.y ||
//		opponent_standard_point.y + opponent_offset.y - opponent_half_length > standard_point.y + offset.y + radius.y)
//	{
//		return false;
//	}
//
//	const Vector3 cylinder_position{ opponent_standard_point + opponent_offset };
//
//	// AABBの範囲内における円柱の中心への最近点を求める
//	Vector3 near_p{ cylinder_position };
//	
//	near_p = near_p.Clamp(standard_point + offset - radius, standard_point + offset + radius);
//
//	DebugPrimitive::GetInstance().DrawSphere(near_p, 0.1f, Vector4(1, 0, 0, 1));
//
//	// 最近点と球の中心の距離の平方が球の半径の平方より小さい場合交差している
//	const float distance{ (cylinder_position - near_p).Length() };
//
//	if (distance < opponent_radius)
//	{
//		out_penetration = opponent_radius - distance;
//
//		return true;
//	}
//
//	return false;
//}

bool ColliderAABB::IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration)
{
	// TODO いるなら実装
	return false;
}
