#include "Weapon.h"

/* --- 追加のヘッダファイルのインクルード ---- */

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


/* --- 定義 ---- */

Weapon::Weapon(int32_t attack_power, int32_t durability)
	: attack_power(attack_power), durability(durability), is_equiped(false), collide_length(1.0f), is_reduced(false)
{
}

void Weapon::Start()
{
	// 親アクタが存在し、そのアクタに WeaponHandler Component があれば、
	// そのアクタに装備されている状態とみなし、WeaponHandlerを呼び出し、自身を設定する
	auto actor{ GetActor() };

	if (const auto& child_param = actor->GetChildActorParam())
	{
		auto parent_actor{ child_param->GetParentActor() };

		if (auto parent_weapon_handler = parent_actor->GetComponent<WeaponHandler>())
		{
			// 装備されている状態
			is_equiped = true;
			parent_weapon_handler->SetWeaponActor(actor);


			// 装備されているときの設定を適用する
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
		// 装備されている状態である
		UpdateIsEquiped(elapsed_time);
	}
	else
	{
		// 装備されていない状態である
		UpdeteIsNotEquiped(elapsed_time);
	}

}

void Weapon::OnHitEvent(const std::shared_ptr<Actor>& opponent)
{
	// 一度その攻撃が命中した敵には当たらない
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

				// フレーム毎一回だけ処理する
				if (!is_reduced)
				{
					// 敵に攻撃が当たったフレームのみエンチャント使用回数を減らす
					weapon_enchant->SubUsuableCount(1);
					is_reduced = true;
				}

				// 使用可能回数を超えたエンチャントは破棄
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

	// 命中した相手をリストに追加
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

	// 装備者の自身に対する参照を切る
	if (const auto& child_param = actor->GetChildActorParam())
	{
		child_param->UnlinkParentActor();
	}

	// 装備されていないときの設定を適用する
	{
		actor->SetGroupName("FieldWeapon");
		actor->GetRigidBody()->SetActiveFlag(true);
		actor->GetCollider("Body")->SetActiveFlag(true);
	}

	// 親アクタの空間に存在していたアクタをワールド空間に移すため、ローカル座標の更新を行う
	actor->SetLocalTransformToWorld();

	// 装備されているフラグを切る
	is_equiped = false;
}

void Weapon::Equip(const std::shared_ptr<Actor>& equip_actor)
{
	auto actor{ GetActor() };

	equip_actor->AddChildActor(actor);

	auto equip_actor_weapon_handler = equip_actor->GetComponent<WeaponHandler>();
	_ASSERT_EXPR(equip_actor_weapon_handler, L"装備するアクタに武器管理コンポーネントがありません");

	if (equip_actor_weapon_handler)
	{
		// 装備するアクタの武器管理コンポーネントに自身のアクタを設定する
		equip_actor_weapon_handler->SetWeaponActor(actor);
	}

	// 装備されているときの設定を適用する
	{
		actor->SetGroupName("Equiped Weapon");
		actor->GetRigidBody()->SetActiveFlag(false);
		actor->GetCollider("Body")->SetActiveFlag(false);
	}

	// 装備されているフラグ
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
	// 交差した敵のリストを初期化
	hit_targets.clear();

	auto actor{ GetActor() };

	auto sphere{ actor->GetCollider<ColliderSphere>("Sphere") };
	auto capsule{ actor->GetCollider<ColliderCapsule>("Capsule") };

	_ASSERT_EXPR(sphere && capsule, L"武器に指定のコライダーを持たせてください。");

	sphere->SetActiveFlag(false);
	capsule->SetActiveFlag(true);

	capsule->SetOffset(offset);
	capsule->SetRadius(radius);

	collide_length = length; // Lengthは武器の向きによって変わる為、別で処理(長さだけ確保)
}

void Weapon::CollideSphere(float radius, const Vector3& offset)
{
	// 交差した敵のリストを初期化
	hit_targets.clear();

	auto actor{ GetActor() };

	auto sphere{ actor->GetCollider<ColliderSphere>("Sphere") };
	auto capsule{ actor->GetCollider<ColliderCapsule>("Capsule") };

	_ASSERT_EXPR(sphere && capsule, L"武器に指定のコライダーを持たせてください。");

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
	// 耐久値がなくなったら破棄
	if (durability < 0)
	{
		GameSystem::GetInstance().GetActorManager()->Remove(GetActor());
		return;
	}

	if (auto weapon_handler = GetActor()->GetChildActorParam()->GetParentActor()->GetComponent<WeaponHandler>())
	{
		if (auto weapon_joint_node = weapon_handler->GetWeaponJointNode())
		{
			// 空間行列の更新
			UpdateTransformToParentWeaponJoint(weapon_joint_node);
		}
	}

}

void Weapon::UpdeteIsNotEquiped(float elapsed_time)
{
	// エフェクトの位置の更新
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
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
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
	//		hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
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

		// スケーリングの抽出
		Vector3	scaling =
		{
			sqrtf(sword_world_transform._11 * sword_world_transform._11 + sword_world_transform._12 * sword_world_transform._12 + sword_world_transform._13 * sword_world_transform._13) / scale.x,
			sqrtf(sword_world_transform._21 * sword_world_transform._21 + sword_world_transform._22 * sword_world_transform._22 + sword_world_transform._23 * sword_world_transform._23) / scale.y,
			sqrtf(sword_world_transform._31 * sword_world_transform._31 + sword_world_transform._32 * sword_world_transform._32 + sword_world_transform._33 * sword_world_transform._33) / scale.z
		};

		// 平行移動行列と回転行列を抽出する、スケーリングは変更しないので不要
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

		// エフェクトの位置の更新
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
