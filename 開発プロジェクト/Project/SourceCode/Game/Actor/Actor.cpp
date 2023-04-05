#include "Actor.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <stack>
#include <algorithm>

#include "imgui.h"

#include "ChildActorParam.h"
#include "Game/Animation/Animator.h"
#include "Game/Component/Component.h"
#include "Game/Behavior/Behavior.h"
#include "Game/Collider/Collider.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/Physics/RigidBody.h"
#include "General/AtaiEngine.h"
#include "Utility/FileDialogBox.h"

/* --- ��` ---- */

Actor::Actor()
	: name(), group_name(),
	world_position(), world_rotation(), world_scale(),
	world_transform(),
	model(),
	components(), behavior(), animator(), rigid_body(),
	w_child_actors(), is_child(false), child_actor_param()
{
}

Actor::~Actor() = default;

void Actor::Initialize(
	const char* name, 
	const Vector3& position, 
	const Quaternion& rotation, 
	const Vector3& scale, 
	const char* group_name) 
{
	SetName(name);
	SetGroupName(group_name);
	SetWorldPosition(position);
	SetWorldRotation(rotation);
	SetWorldScale(scale);

	// �A�N�^���q�ł���ꍇ�͂��̃��[�J�����W�ɐݒ肷��
	if (is_child)
	{
		child_actor_param->local_position = position;
		child_actor_param->local_rotation = rotation;
		child_actor_param->local_scale = scale;
	}
	else
	{
		world_position = position;
		world_rotation = rotation;
		world_scale = scale;
	}
}

void Actor::Start()
{
	if (behavior) behavior->Start();
	if (animator) animator->Start();
	if (rigid_body) rigid_body->Start();

	for (auto& c : colliders)
	{
		c.second->Start();
	}

	for (auto& c : components)
	{
		c->Start();
	}
}

void Actor::Update(float elapsed_time)
{
	for (auto& c : components)
	{
		// ���s�\�ȃR���|�[�l���g��
		if (c->IsActive())
		{
			c->Update(elapsed_time);
		}
	}

	// ���g�̎q�A�N�^�̍X�V�������s��
	for (const auto& w_ca : w_child_actors)
	{
		w_ca.lock()->Update(elapsed_time);
	}
}

void Actor::End()
{
	if (behavior) behavior->End();
	if (animator) animator->End();
	if (rigid_body)rigid_body->End();

	for (auto& c : colliders)
	{
		c.second->End();
	}

	for (auto& c : components)
	{
		c->End();
	}
}


void Actor::UpdateTransform(const Matrix& render_model_coordinate_space)
{
	// ���[�v���ōs�񂪐ݒ肳��Ă���ꍇ���̍s��ɍX�V����B
	if (next_transform.has_value())
	{
		const auto& transform{ next_transform.value() };

		transform.DecomposeTransform(world_position, world_rotation, world_scale);
		world_transform = transform;
	}
	else
	{
		world_transform = Matrix::CreateTransform(world_position, world_rotation, world_scale);
	}

	// ���f���̍s��X�V
	if (model != nullptr)
	{
		model->SetWorldTransform(world_transform, render_model_coordinate_space);
	}

	// ���g�̎q�A�N�^�̍s����X�V����B
	for (const auto& w_ca : w_child_actors)
	{
		w_ca.lock()->UpdateTransformAsChild(world_transform);
	}

	next_transform.reset();
}

bool Actor::CompareName(const char* opponent) const
{
	return std::hash<std::string>()(name) == std::hash<std::string>()(opponent);
}

bool Actor::CompareGroupName(const char* opponent) const
{
	return std::hash<std::string>()(group_name) == std::hash<std::string>()(opponent);
}

void Actor::UpdateTransformAsChild(const Matrix& parent_coordinate)
{
	auto& c{ child_actor_param };

	// ���[�v���ōs�񂪐ݒ肳��Ă���ꍇ���̍s��ɍX�V����B
	if (next_transform.has_value())
	{
		const auto& transform{ next_transform.value() };

		transform.DecomposeTransform(c->local_position, c->local_rotation, c->local_scale);
		c->local_transform = transform;
	}
	else
	{
		c->local_transform = Matrix::CreateTransform(c->local_position, c->local_rotation, c->local_scale);
	}

	// �Q�Ƃ���̂͐e�̈ʒu�̂�
	if (c->conform_position_only)
	{
		world_transform = c->local_transform;
		world_transform.m[3][0] += parent_coordinate.m[3][0];
		world_transform.m[3][1] += parent_coordinate.m[3][1];
		world_transform.m[3][2] += parent_coordinate.m[3][2];
	}
	else
	{
		world_transform = c->local_transform * parent_coordinate;
	}


	world_transform.DecomposeTransform(world_position, world_rotation, world_scale);

	// ���f���̍s��X�V
	if (model != nullptr)
	{
		model->SetWorldTransform(world_transform);
	}

	// ���g�̎q�A�N�^�̍s����X�V����B
	for (const auto& w_ca : w_child_actors)
	{
		w_ca.lock()->UpdateTransformAsChild(world_transform);
	}

	next_transform.reset();
}

void Actor::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) const
{
	// if (behavior_statemachine) behavior_statemachine->OnHitEvent(opponent_actor);

	for (const auto& component : components)
	{
		component->OnHitEvent(opponent_actor);
	}
}

void Actor::ExecuteCurrentPlayAnimationEvent()
{
	if (animator)
	{
		animator->AnimationEvent();
	}
}


void Actor::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if (model != nullptr)
	{
		model->Render(dc, shader);
	}
}

void Actor::UpdateBehavior(float elapsed_time) const
{
	if (behavior)
	{
		behavior->Update(elapsed_time);
	}
}

void Actor::UpdateAnimation(float elapsed_time) const
{
	if (animator)
	{
		animator->Update(elapsed_time);
	}
}

void Actor::UpdateRigidBody(float elapsed_time) const
{
	if (rigid_body)
	{
		rigid_body->Update(elapsed_time);
	}
}

void Actor::UpdateCollider(float elapsed_time) const
{
	for (auto& collider : colliders)
	{
		collider.second->Update(elapsed_time);
	}
}

std::shared_ptr<Animator> Actor::CreateAnimator()
{
	animator = std::make_shared<Animator>();
	animator->SetActor(shared_from_this());
	return animator;
}

std::shared_ptr<RigidBody> Actor::CreateRigidBody(float mass, bool is_effect_gravity)
{
	rigid_body = std::make_shared<RigidBody>(mass, is_effect_gravity);
	rigid_body->SetActor(shared_from_this());
	return rigid_body;
}

void Actor::RegisterCollider(const std::shared_ptr<Collider>& collider)
{
	GameSys.GetColliderManager()->RegisterCollider(collider);
}

std::shared_ptr<Animator> Actor::GetAnimator()
{
	return animator;
}

std::shared_ptr<RigidBody> Actor::GetRigidBody()
{
	return rigid_body;
}

const std::unordered_map<std::string, std::shared_ptr<Collider>>& Actor::GetColliders() const
{
	return colliders;
}

void Actor::UpdateActorRelation()
{
	// �Q�Ƃ��؂ꂽ�q�A�N�^��z�񂩂珜�O����
	auto itr_erase_expired_child_actors{
		std::ranges::remove_if(w_child_actors, [](const auto& wptr) {return wptr.expired(); }) };
	w_child_actors.erase(itr_erase_expired_child_actors.begin(), w_child_actors.end());

	// �Q�Ƃ�؂�q�A�N�^
	for (const auto& remove_child_actor : remove_child_actors)
	{
		auto itr{ std::ranges::remove_if(w_child_actors,
				   [remove_child_actor](const auto& wptr) { return wptr.lock() == remove_child_actor; }) };
		w_child_actors.erase(itr.begin(), w_child_actors.end());
	}

	remove_child_actors.clear();

	// �q�A�N�^�̎�
	if (child_actor_param)
	{
		child_actor_param->UpdateRelationToParent();
	}
}

void Actor::SetParentActor(const std::shared_ptr<Actor>& actor)
{
	if (actor)
	{
		child_actor_param = std::make_unique<ChildActorParam>(shared_from_this(), actor);
		is_child = true;
	}
	else
	{
		child_actor_param.reset();
		is_child = false;
	}

}

void Actor::AddChildActor(const std::shared_ptr<Actor>& actor)
{
	actor->SetParentActor(shared_from_this());

	w_child_actors.emplace_back(actor);
	//// �e�q�̌Z��Ƃ��ēo�^����
	//AddSiblingOfChildren(actor);
}

void Actor::RemoveChildActor(const std::shared_ptr<Actor>& actor)
{
	remove_child_actors.emplace(actor);

	UpdateActorRelation();
	// �q���猩�Đe�̎Q�Ƃ�؂�
	actor->SetParentActor(nullptr);
}

// GUI�\��
void Actor::ImGuiRender()
{
	// ���O
	{
		char buffer[64];
		::strncpy_s(buffer, sizeof(buffer), GetName().c_str(), sizeof(buffer));
		if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly))
		{
			SetName(buffer);
		}

		::strncpy_s(buffer, sizeof(buffer), GetGroupName().c_str(), sizeof(buffer));
		if (ImGui::InputText("Group", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly))
		{
			SetName(buffer);
		}
	}

	if (is_child)
	{
		if (ImGui::CollapsingHeader("LocalTransform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("L_Position", &child_actor_param->local_position.x);
			ImGui::InputFloat4("L_Rotation", &child_actor_param->local_rotation.x);
			ImGui::InputFloat3("L_Scale", &child_actor_param->local_scale.x);
		}
	}

	if (ImGui::CollapsingHeader("WorldTransform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat3("W_Position", &world_position.x);
		ImGui::InputFloat4("W_Rotation", &world_rotation.x);
		ImGui::InputFloat3("W_Scale", &world_scale.x);
	}

	// ���f��
	if (model)
	{
		if(ImGui::Button(TO_CHAR(u8"���f���̕ύX(���f���ɂ���Ă̓G���[)")))
		{
			auto str{ FileDialogBox::SelectPathA(FileDialogBox::Open, "*.gltf\0*.gltf\0*glb\0*glb\0", ".\\Data\\") };
			MySystem.RestoreCurrentDirctory();

			if (!str.empty())
			{
				model = std::make_unique<GLTFSkinnedMesh>(MySystem.GetDevice(), str.c_str());
			}
		}
	}

	// �r�w�C�r�A
	if (behavior)
	{
		behavior->ImGuiRender();
	}

	// �R���C�_�[
	for(const auto& collider : colliders)
	{
		collider.second->ImGuiRender();
	}

	// �R���|�[�l���g
	for (auto& component : components)
	{
		ImGui::Spacing();
		ImGui::Separator();

		if (ImGui::TreeNodeEx(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::PushID(StringToHash(component->GetName()));
			component->ImGuiRender();
			//ImGui::PopID();

			ImGui::TreePop();
		}
	}

	// ���g��j��
	{
		if (ImGui::Button("---Destroy---"))
		{
			GameSys.GetActorManager()->Remove(shared_from_this());
		}
	}
}
