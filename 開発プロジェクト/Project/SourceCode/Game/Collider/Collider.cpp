#include "Collider.h"


/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "../Actor/Actor.h"

#include "ColliderAABB.h"
#include "ColliderSphere.h"
//#include "ColliderVCylinder.h"
#include "ColliderCapsule.h"

/* --- 定義 ---- */

std::map<size_t, Collider::Type> Collider::collider_type_hash_list = {};

void Collider::Start()
{
}

void Collider::Update(float elapsed_time)
{
	if (auto node = reference_node.lock())
	{
		standard_point = (node->global_transform * GetActor()->GetModel<GLTFModel>()->GetWorldTransform()).m[3];
	}
	else
	{
		standard_point = GetActor()->GetWorldPosition();
	}
}

void Collider::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}

void Collider::ImGuiRender()
{
	if (ImGui::TreeNode(std::format("Collider_{}", name).c_str()))
	{
		ImGui::Checkbox("DebugPrimitive", &show_debug_primitive);

		ImGui::ListBoxHeader(TO_CHAR(u8"設定された関数名"));
		for (const auto& func : on_hit_event_call_back)
		{
			ImGui::Text(func.first.c_str());
		}
		ImGui::ListBoxFooter();

		ImGui::TreePop();
	}
}

void Collider::SetReferenceNode(const char* node_name)
{
	reference_node = GetActor()->GetModel<GLTFSkinnedMesh>()->FindAnimatedNode(node_name);
}

void Collider::InitializeHashList()
{
	collider_type_hash_list[typeid(ColliderAABB).hash_code()] = Type::AABB;
	collider_type_hash_list[typeid(ColliderSphere).hash_code()] = Type::Sphere;
	//collider_type_hash_list[typeid(ColliderVCylinder).hash_code()] = Type::VCylinder;
	collider_type_hash_list[typeid(ColliderCapsule).hash_code()] = Type::Capsule;
}

void Collider::OnHitEventCallback(const std::shared_ptr<Actor>& opponent)
{
	auto actor{ GetActor() };

	for (const auto& func : on_hit_event_call_back)
	{
		func.second(actor, opponent);
	}
}

Collider::Type Collider::GetColliderType(const size_t& hash_code)
{
	return collider_type_hash_list.at(hash_code);
}

bool Collider::Intersect(const std::shared_ptr<Collider>& opponent_collider)
{
	float pad;

	return Intersect(opponent_collider, pad);
}

bool Collider::Intersect(const std::shared_ptr<Collider>& opponent_collider, float& out_penetration)
{
	if (collider_type_hash_list.empty()) InitializeHashList();

	// 相手のコライダーの種類を判別する
	switch (GetColliderType(opponent_collider->GetHash()))
	{
	case Type::AABB:
		// AABBとの交差
		return IntersectAABB(std::dynamic_pointer_cast<ColliderAABB>(opponent_collider), out_penetration);
	case Type::Sphere:
		// 球との交差
		return IntersectSphere(std::dynamic_pointer_cast<ColliderSphere>(opponent_collider), out_penetration);
	//case Type::VCylinder:
	//	// 垂直円柱との交差
	//	return IntersectVCylinder(std::dynamic_pointer_cast<ColliderVCylinder>(opponent_collider), out_penetration);
	case Type::Capsule:
		// カプセルとの交差
		return IntersectCapsule(std::dynamic_pointer_cast<ColliderCapsule>(opponent_collider), out_penetration);
	default:
		_ASSERT_EXPR(0, L"未知のコライダーです");
		return false;
	}

	return false;
}

void Collider::AddOnHitEventCallBack(
	const char* func_name, 
	const std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>& callback)
{
	_ASSERT_EXPR(!on_hit_event_call_back.contains(func_name), L"既に同じ名前のコールバック関数が設定されています");
	on_hit_event_call_back[func_name] = callback;
}

void Collider::RemoveOnHitEventCallBack(const char* func_name)
{
	on_hit_event_call_back.erase(func_name);
}

void Collider::ClearOnHitEventCallBack()
{
	on_hit_event_call_back.clear();
}
