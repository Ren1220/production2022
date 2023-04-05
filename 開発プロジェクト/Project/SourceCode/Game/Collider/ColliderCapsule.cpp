#include "ColliderCapsule.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "IntersectUser/Intersect.h"
#include "ColliderAABB.h"
#include "ColliderSphere.h"


/* --- 定義 ---- */

void ColliderCapsule::Start()
{
}

void ColliderCapsule::Update(float elapsed_time)
{
	Collider::Update(elapsed_time);

	// デバッグ用プリミティブの描画
	if (show_debug_primitive && is_active)
	{
		DebugPrimitive::GetInstance().DrawCapsule(standard_point + offset, standard_point + offset + length, radius, color);
	}
}

void ColliderCapsule::ImGuiRender()
{
	Collider::ImGuiRender();
}

bool ColliderCapsule::IntersectSphere(
	const Vector3& sphere_position, 
	float sphere_radius) const
{
	float dummy;

	if (IntersectUser::IntersectSphereVSCapsule(
		sphere_position,
		sphere_radius,
		standard_point,
		standard_point + length,
		radius, 
		dummy))
		{
			return true;
		}

	return false;
}

bool ColliderCapsule::IntersectAABB(
	const std::shared_ptr<ColliderAABB>& opponent, 
	float& out_penetration)
{
	// 必要なら実装
	return false;
}

bool ColliderCapsule::IntersectSphere(
	const std::shared_ptr<ColliderSphere>& opponent, 
	float& out_penetration)
{
	float dist;

	if (IntersectUser::IntersectSphereVSCapsule(
		opponent->GetStandardPoint(),
		opponent->GetRadius(),
		standard_point,
		standard_point + length,
		radius,
		dist))
	{
		out_penetration = radius + opponent->GetRadius() - dist;
		return true;
	}

	return false;
}

bool ColliderCapsule::IntersectCapsule(
	const std::shared_ptr<ColliderCapsule>& opponent,
	float& out_penetration)
{
	Vector3 t_near, o_near;

	if (IntersectUser::IntersectCapsuleVSCapsule(
		standard_point,
		standard_point + length,
		radius,
		opponent->standard_point,
		opponent->standard_point + opponent->length,
		opponent->radius,
		t_near,
		o_near))
	{
		out_penetration = (o_near - t_near).Length() - (radius + opponent->radius);
		return true;
	}

	return false;
}
