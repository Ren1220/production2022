#include "Weapon.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <ranges>

#include "Audio/AudioHandle.h"
#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "../Health/Health.h"

#include "../../Actor/Actor.h"
#include "../../Actor/ActorPlayer.h"
#include "../../Actor/ChildActorParam.h"
#include "../../Behavior/StateMachine/StateMachine.h"
#include "../../Collider/Collider.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderManager.h"
#include "../../Physics/RigidBody.h"
#include "../../General/BattleSystem.h"
//#include "ColliderSphere.h"
//#include "CollideHandler.h"
//#include "Effect/SwordTrail.h"
#include "WeaponHandler.h"
#include "Enchant/WeaponEnchantList.h"


/* --- ��` ---- */

Weapon::Weapon(int32_t attack_power, int32_t durability)
	: attack_power(attack_power), durability(durability), is_equiped(false), collide_length(1.0f), is_reduced(false)
{
}

void Weapon::Start()
{
	// �e�A�N�^�����݂��A���̃A�N�^�� WeaponHandler Component ������΁A
	// ���̃A�N�^�ɑ�������Ă����ԂƂ݂Ȃ��AWeaponHandler���Ăяo���A���g��ݒ肷��
	auto actor{ GetActor() };

	if (const auto& child_param = actor->GetChildActorParam())
	{
		auto parent_actor{ child_param->GetParentActor() };

		if (auto parent_weapon_handler = parent_actor->GetComponent<WeaponHandler>())
		{
			// ��������Ă�����
			is_equiped = true;
			parent_weapon_handler->SetWeaponActor(actor);


			// ��������Ă���Ƃ��̐ݒ��K�p����
			{
				actor->GetRigidBody()->SetActiveFlag(false);
				NoCollide();
			}
		}
	}

}

void Weapon::End()
{
	if (weapon_enchant)
	{
		weapon_enchant->EndProcess();
	}
}

void Weapon::Update(float elapsed_time)
{
	is_reduced = false;

	if (is_equiped)
	{
		// ��������Ă����Ԃł���
		UpdateIsEquiped(elapsed_time);
	}
	else
	{
		// ��������Ă��Ȃ���Ԃł���
		UpdeteIsNotEquiped(elapsed_time);
	}

}

void Weapon::OnHitEvent(const std::shared_ptr<Actor>& opponent)
{
	// ��x���̍U�������������G�ɂ͓�����Ȃ�
	if (hit_targets.contains(opponent->GetName()))
	{
		return;
	}

	if (GetOwner()->CompareGroupName("Player"))
	{
		if (opponent->CompareGroupName("Enemy") || opponent->CompareGroupName("Boss"))
		{
			if (weapon_enchant)
			{
				weapon_enchant->AttackByPlayer(GetOwner(), opponent, attack_power);

				// �t���[������񂾂���������
				if (!is_reduced)
				{
					// �G�ɍU�������������t���[���̂݃G���`�����g�g�p�񐔂����炷
					weapon_enchant->SubUsuableCount(1);
					is_reduced = true;
				}

				// �g�p�\�񐔂𒴂����G���`�����g�͔j��
				if (weapon_enchant->GetUsuableCount() < 0)
				{
					SetWeaponEnchant(nullptr);
				}
			}
			else
			{
				AttackByPlayer(GetOwner(), opponent);
			}
		}
	}
	else if(GetOwner()->CompareGroupName("Enemy"))
	{
		if (opponent->CompareGroupName("Player"))
		{
			if (weapon_enchant)
			{
				weapon_enchant->Attack(shared_from_this(), opponent, attack_power);
			}
			else
			{
				BattleSystem::GetInstance().SendCombatData(GetOwner(), opponent, attack_power, 1.0f, 0.2f);
			}
		}
	}
	else
	{
		assert(0);
	}

	// ����������������X�g�ɒǉ�
	hit_targets.emplace(opponent->GetName());
}

void Weapon::ImGuiRender()
{
#ifdef _DEBUG


	if (ImGui::TreeNode("Enchant"))
	{
		auto list{ WeaponEnchantList::GetNames() };

		ImGui::ListBox("List", &select_enchant, list.data(), static_cast<int>(list.size()));

		if (ImGui::Button("Apply"))
		{
			WeaponEnchantList::SetEnchantFromList(static_cast<WeaponEnchantList::List>(select_enchant), weapon_enchant);
			if (weapon_enchant)
			{
				weapon_enchant->Initialize(Vector3::ZeroVector());
			}
		}

		if (weapon_enchant)
		{
			//weapon_enchant->ImGuiRender();
		}

		ImGui::TreePop();
	}
#endif // DEBUG
}

void Weapon::Unequip()
{
	auto actor{ GetActor() };

	// �����҂̎��g�ɑ΂���Q�Ƃ�؂�
	if (const auto& child_param = actor->GetChildActorParam())
	{
		child_param->UnlinkParentActor();
	}

	// ��������Ă��Ȃ��Ƃ��̐ݒ��K�p����
	{
		actor->SetGroupName("FieldWeapon");
		actor->GetRigidBody()->SetActiveFlag(true);
		actor->GetCollider("Body")->SetActiveFlag(true);
	}

	// �e�A�N�^�̋�Ԃɑ��݂��Ă����A�N�^�����[���h��ԂɈڂ����߁A���[�J�����W�̍X�V���s��
	actor->SetLocalTransformToWorld();

	// ��������Ă���t���O��؂�
	is_equiped = false;
}

void Weapon::Equip(const std::shared_ptr<Actor>& equip_actor)
{
	auto actor{ GetActor() };

	equip_actor->AddChildActor(actor);

	auto equip_actor_weapon_handler = equip_actor->GetComponent<WeaponHandler>();
	_ASSERT_EXPR(equip_actor_weapon_handler, L"��������A�N�^�ɕ���Ǘ��R���|�[�l���g������܂���");

	if (equip_actor_weapon_handler)
	{
		// ��������A�N�^�̕���Ǘ��R���|�[�l���g�Ɏ��g�̃A�N�^��ݒ肷��
		equip_actor_weapon_handler->SetWeaponActor(actor);
	}

	// ��������Ă���Ƃ��̐ݒ��K�p����
	{
		actor->SetGroupName("Equiped Weapon");
		actor->GetRigidBody()->SetActiveFlag(false);
		actor->GetCollider("Body")->SetActiveFlag(false);
	}

	// ��������Ă���t���O
	is_equiped = true;
}

void Weapon::CollideCombo0()
{
	if (weapon_enchant)
	{
		weapon_enchant->CollideCombo0(std::dynamic_pointer_cast<Weapon>(shared_from_this()));
	}
	else
	{
		CollideCapsule(1.0f, 0.25f);
	}
}

void Weapon::CollideCombo1()
{
	if (weapon_enchant)
	{
		weapon_enchant->CollideCombo1(std::dynamic_pointer_cast<Weapon>(shared_from_this()));
	}
	else
	{
		CollideCapsule(1.0f, 0.25f);
	}
}

void Weapon::CollideCombo2()
{
	if (weapon_enchant)
	{
		weapon_enchant->CollideCombo2(std::dynamic_pointer_cast<Weapon>(shared_from_this()));
	}
	else
	{
		CollideCapsule(1.0f, 0.25f);
	}
}

void Weapon::CollideSlash()
{
	if (weapon_enchant)
	{
		weapon_enchant->CollideSlash(std::dynamic_pointer_cast<Weapon>(shared_from_this()));
	}
	else
	{
		CollideCapsule(1.0f, 0.25f);
	}
}

void Weapon::CollideJumpATK()
{
	if (weapon_enchant)
	{
		weapon_enchant->CollideJumpAtk(std::dynamic_pointer_cast<Weapon>(shared_from_this()));
	}
	else
	{
		CollideCapsule(1.0f, 0.25f);
	}
}

void Weapon::NoCollide()
{
	for (const auto& c : GetActor()->GetColliders())
	{
		c.second->SetActiveFlag(false);
	}
}

void Weapon::CollideCapsule(float length, float radius, const Vector3& offset)
{
	// ���������G�̃��X�g��������
	hit_targets.clear();

	auto actor{ GetActor() };

	auto sphere{ actor->GetCollider<ColliderSphere>("Sphere") };
	auto capsule{ actor->GetCollider<ColliderCapsule>("Capsule") };

	_ASSERT_EXPR(sphere && capsule, L"����Ɏw��̃R���C�_�[���������Ă��������B");

	sphere->SetActiveFlag(false);
	capsule->SetActiveFlag(true);

	capsule->SetOffset(offset);
	capsule->SetRadius(radius);

	collide_length = length; // Length�͕���̌����ɂ���ĕς��ׁA�ʂŏ���(���������m��)
}

void Weapon::CollideSphere(float radius, const Vector3& offset)
{
	// ���������G�̃��X�g��������
	hit_targets.clear();

	auto actor{ GetActor() };

	auto sphere{ actor->GetCollider<ColliderSphere>("Sphere") };
	auto capsule{ actor->GetCollider<ColliderCapsule>("Capsule") };

	_ASSERT_EXPR(sphere && capsule, L"����Ɏw��̃R���C�_�[���������Ă��������B");

	sphere->SetActiveFlag(true);
	capsule->SetActiveFlag(false);

	sphere->SetRadius(radius);
	sphere->SetOffset(offset);
}

void Weapon::CreateTrail(const wchar_t* trail_texture_path)
{
	//auto sword_trail{ std::make_shared<SwordTrail>(GetActor(), trail_texture_path) };
	//ActorManager::GetInstance().GetEffectManager().Register(sword_trail);
	//w_trail_effect = sword_trail;
}

void Weapon::SetWeaponEnchant(std::unique_ptr<WeaponEnchant>&& enchant)
{
	if (enchant)
	{
		if (weapon_enchant)
		{
			weapon_enchant->EndProcess();
		}

		weapon_enchant = std::move(enchant);
		weapon_enchant->Initialize(GetActor()->GetWorldPosition());
	}
	else
	{
		if (weapon_enchant)
		{
			weapon_enchant->EndProcess();
			weapon_enchant.reset();
		}
	}
}

//std::shared_ptr<EffectBase> Weapon::GetTrailEffect() const
//{
//	return w_trail_effect.lock();
//}

void Weapon::SubDurability(int32_t val)
{
	//durability -= val * GetOwner()->GetComponent<WeaponHandler>()->GetDuraRedReflection();
}

void Weapon::UpdateIsEquiped(float elapsed_time)
{
	// �ϋv�l���Ȃ��Ȃ�����j��
	if (durability < 0)
	{
		GameSystem::GetInstance().GetActorManager()->Remove(GetActor());
		return;
	}

	if (auto weapon_handler = GetActor()->GetChildActorParam()->GetParentActor()->GetComponent<WeaponHandler>())
	{
		if (auto weapon_joint_node = weapon_handler->GetWeaponJointNode())
		{
			// ��ԍs��̍X�V
			UpdateTransformToParentWeaponJoint(weapon_joint_node);
		}
	}

}

void Weapon::UpdeteIsNotEquiped(float elapsed_time)
{
	// �G�t�F�N�g�̈ʒu�̍X�V
	if (weapon_enchant)
	{
		if (const auto& effect = weapon_enchant->GetWeaponEffect())
		{
			effect->SetPosition(GetActor()->GetWorldPosition());
		}
	}
}

void Weapon::CollideToAttackTarget(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node)
{
}

void Weapon::AttackByPlayer(
	const std::shared_ptr<Actor>& owner, 
	const std::shared_ptr<Actor>& target)
{
	auto behavior{ owner->GetBehavior<StateMachine>() };

	if (!behavior->GetCurrentSubStateKey().has_value()) return;

	ActorPlayer::State state{ static_cast<ActorPlayer::State>(behavior->GetCurrentSubStateKey().value()) };

	int32_t damage_val = 0;

	switch (state)
	{
	case ActorPlayer::_AttackCombo0:
		damage_val = 10;
		break;
	case ActorPlayer::_AttackCombo1:
		damage_val = 10;
		break;
	case ActorPlayer::_AttackCombo2:
		damage_val = 20;
		break;
	case ActorPlayer::_AttackSlash:
		damage_val = 20;
		break;
	case ActorPlayer::_JumpAttackState:
		damage_val = 30;
		break;
	}

	BattleSystem::GetInstance().SendCombatData(owner, target, damage_val, 0.0f, 0.2f,
		[](Actor* owner, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle slash_efk("Slash/slash.efk", target->GetWorldPosition() + Vector3(0, 1, 0));
				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/���ʉ����{/���Ŏa��2.wav");
			}
		});

	//if (auto health = collided_target->GetComponent<Health>())
	//{
	//	EffekseerHandle slash_efk("Slash/slash.efk", collided_target->GetWorldPosition() + Vector3(0, 1, 0));

	//	//	hit.ReverbEffect();



	//	if (health->Damaged(GetOwner(), attack_power, 0.1f, 0.2f))
	//	{
	//		SubDurability(1);
	//		AudioHandle hit;
	//		hit.Play(L"Data/Audio/SE/���ʉ����{/���Ŏa��2.wav");
	//	}
	//}
}

std::shared_ptr<Actor> Weapon::GetOwner() const
{
	const auto& param{ GetActor()->GetChildActorParam() };
	return param ? param->GetParentActor() : nullptr;
}

void Weapon::UpdateTransformToParentWeaponJoint(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node)
{
	if (const auto& node = weapon_joint_node)
	{
		auto actor{ GetActor() };

		const auto& sword_world_transform = node->global_transform;
		const Vector3& scale{ actor->GetWorldScale() };

		// �X�P�[�����O�̒��o
		Vector3	scaling =
		{
			sqrtf(sword_world_transform._11 * sword_world_transform._11 + sword_world_transform._12 * sword_world_transform._12 + sword_world_transform._13 * sword_world_transform._13) / scale.x,
			sqrtf(sword_world_transform._21 * sword_world_transform._21 + sword_world_transform._22 * sword_world_transform._22 + sword_world_transform._23 * sword_world_transform._23) / scale.y,
			sqrtf(sword_world_transform._31 * sword_world_transform._31 + sword_world_transform._32 * sword_world_transform._32 + sword_world_transform._33 * sword_world_transform._33) / scale.z
		};

		// ���s�ړ��s��Ɖ�]�s��𒊏o����A�X�P�[�����O�͕ύX���Ȃ��̂ŕs�v
		Matrix translation_rotation =
		{
			sword_world_transform._11 / scaling.x, sword_world_transform._12 / scaling.x, sword_world_transform._13 / scaling.x, 0.0f,
			sword_world_transform._21 / scaling.y, sword_world_transform._22 / scaling.y, sword_world_transform._23 / scaling.y, 0.0f,
			sword_world_transform._31 / scaling.z, sword_world_transform._32 / scaling.z, sword_world_transform._33 / scaling.z, 0.0f,
			sword_world_transform._41			 , sword_world_transform._42			, sword_world_transform._43			   , 1.0f
		};

		
		translation_rotation = translation_rotation  * Matrix::RhsYup();
		Matrix world{ translation_rotation * actor->GetChildActorParam()->GetParentActor()->GetWorldTransform() };

		actor->SetNextTransform(translation_rotation);

		actor->GetCollider<ColliderCapsule>("Capsule")->SetLength(Vector3(world.m[2]) * collide_length);

		// �G�t�F�N�g�̈ʒu�̍X�V
		if (weapon_enchant)
		{
			const auto& effect{ weapon_enchant->GetWeaponEffect() };

			//weapon_enchant->UpdateTransform(
			//	Vector3(sword_world_transform.m[3]) + actor->GetChildActorParam()->GetParentActor()->GetWorldPosition(),
			//	{ 0, 0, 0 },   
			//	{ 1, 1, 1 });


			effect->SetPosition(Vector3(world.m[2]) * 0.5f + Vector3( world.m[3]));
			effect->SetRotation(world.m[2], 0.0f);
			effect->SetScale({ 0.5f, 0.5f, 0.5f });
		}

	}
}
