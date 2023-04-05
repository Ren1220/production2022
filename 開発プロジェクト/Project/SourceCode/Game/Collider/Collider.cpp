#include "Collider.h"


/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "../Actor/Actor.h"

#include "ColliderAABB.h"
#include "ColliderSphere.h"
//#include "ColliderVCylinder.h"
#include "ColliderCapsule.h"

/* --- ��` ---- */

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

		ImGui::ListBoxHeader(TO_CHAR(u8"�ݒ肳�ꂽ�֐���"));
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

	// ����̃R���C�_�[�̎�ނ𔻕ʂ���
	switch (GetColliderType(opponent_collider->GetHash()))
	{
	case Type::AABB:
		// AABB�Ƃ̌���
		return IntersectAABB(std::dynamic_pointer_cast<ColliderAABB>(opponent_collider), out_penetration);
	case Type::Sphere:
		// ���Ƃ̌���
		return IntersectSphere(std::dynamic_pointer_cast<ColliderSphere>(opponent_collider), out_penetration);
	//case Type::VCylinder:
	//	// �����~���Ƃ̌���
	//	return IntersectVCylinder(std::dynamic_pointer_cast<ColliderVCylinder>(opponent_collider), out_penetration);
	case Type::Capsule:
		// �J�v�Z���Ƃ̌���
		return IntersectCapsule(std::dynamic_pointer_cast<ColliderCapsule>(opponent_collider), out_penetration);
	default:
		_ASSERT_EXPR(0, L"���m�̃R���C�_�[�ł�");
		return false;
	}

	return false;
}

void Collider::AddOnHitEventCallBack(
	const char* func_name, 
	const std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>& callback)
{
	_ASSERT_EXPR(!on_hit_event_call_back.contains(func_name), L"���ɓ������O�̃R�[���o�b�N�֐����ݒ肳��Ă��܂�");
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
