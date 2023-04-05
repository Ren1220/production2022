#include "ActorWolf.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../Actor/ActorPlayer.h"
#include "../Actor/ActorManager.h"
#include "../Animation/Animator.h"
#include "../Physics/RigidBody.h"
#include "../Behavior/BehaviorTree/Enemy/BehaviorWolf.h"
#include "../Behavior/StateMachine/StateMachine.h"
#include "../Behavior/StateMachine/StateMachineStatePlayer.h"
#include "../Collider/ColliderCapsule.h"
//#include "FaceDirection.h"
#include "../Component/Health/Health.h"
#include "../Component/Character.h"	
#include "../Component/Weapon/Weapon.h"
#include "../Component/Weapon/WeaponHandler.h"
#include "../Collider/ColliderManager.h"
#include "../General/GameSystem.h"
#include "../General/BattleSystem.h"

/* --- 定義 ---- */

void ActorWolf::Initialize(
	ID3D11Device* device,
	const char* name,
	const Vector3& position, 
	const Quaternion& rotation, 
	const Vector3& scale,
	bool has_summoned)
{
	Actor::Initialize<GLTFSkinnedMesh>(
		device,
		name,
		position,
		rotation,
		scale,
		"Enemy",
		"Data/Model/GLTF/Enemy/Wolf/wolf3.gltf");

	RegisterAnimation();

	CreateRigidBody(1.0f, true);


	CreateBehavior<BehaviorWolf>(has_summoned);

	AddComponent<Health>(100, 100);
	auto character{ AddComponent<Character>() };
	character->SetRadius(0.3f * scale.x);
	character->SetHeight(1.5f * scale.y);

	CreateCollider<ColliderCapsule>("Body", Vector3(0, 0, 0), Vector3(0, character->GetHeight(), 0), character->GetRadius(), Vector4(0, 1, 0, 1));
}

void ActorWolf::PlayAppearanceEffect()
{
	appearance_effect.Initialize("Summon/appearance_wolf.efk", world_position);
	appearance_effect.SetScale({ 0.5f,0.5f,0.5f });
}

void ActorWolf::DestroyAppearanceEffect()
{
	appearance_effect.Destroy();
}

void ActorWolf::RegisterAnimation()
{
	auto model_skinned{ GetModel<GLTFSkinnedMesh>() };
	auto animator{ CreateAnimator() };
	animator->RegisterAnimation(Run, 0);
	animator->RegisterAnimation(Walk, 1);
	animator->RegisterAnimation(CrouchWalk, 2);
	animator->RegisterAnimation(Idle, 3);
	animator->RegisterAnimation(Sit, 4);
	const auto& anim_appearance{ animator->RegisterAnimation(Appearance, 5) };
	const auto& anim_attack{ animator->RegisterAnimation(Attack, 6) };
	const auto& anim_attack2{ animator->RegisterAnimation(Attack2, 7) };
	animator->RegisterAnimation(Damaged, 8);
	const auto& anim_dead{ animator->RegisterAnimation(Dead, 9) };
	animator->RegisterAnimation(Howling, 10);

	anim_appearance->SetEventWhenEnter(
		[](const std::shared_ptr<Actor>& actor)
		{
			std::dynamic_pointer_cast<ActorWolf>(actor)->PlayAppearanceEffect();
		});
	anim_appearance->SetEventWhenExit(
		[](const std::shared_ptr<Actor>& actor)
		{
			std::dynamic_pointer_cast<ActorWolf>(actor)->DestroyAppearanceEffect();
		});

	anim_attack->AddEventPerFrame(
		[](const std::shared_ptr<Actor>& actor)
		{
			Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };
			Vector3 p{ actor->GetWorldPosition() };
			p += f;

			auto hit_player{ GameSys.GetColliderManager()->GetCollidedActorsWithSphere(p, 1.0f, "Player") };
			for (const auto& p : hit_player)
			{
				BattleSystem::GetInstance().SendCombatData(actor, p, 10, 1.0f, 0.5f);
			}

		}, 0.4f, 0.5f);
	anim_attack2->AddEventPerFrame(
		[](const std::shared_ptr<Actor>& actor)
		{
			Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };
			Vector3 p{ actor->GetWorldPosition() };
			p += f;

			auto hit_player{ GameSys.GetColliderManager()->GetCollidedActorsWithSphere(p, 1.0f, "Player") };
			for (const auto& p : hit_player)
			{
				BattleSystem::GetInstance().SendCombatData(actor, p, 10, 1.0f, 0.5f);
			}

		}, 0.1f, 0.2f);
	anim_dead->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			GameSys.GetActorManager()->Remove(actor);
		}, anim_dead->MeasureAnimationTimeEnd(model_skinned));

}
