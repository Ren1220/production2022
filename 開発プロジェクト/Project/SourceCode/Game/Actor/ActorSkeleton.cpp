#include "ActorSkeleton.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../Actor/ActorPlayer.h"
#include "../Animation/Animator.h"
#include "../Physics/RigidBody.h"
#include "../Behavior/BehaviorTree/Enemy/BehaviorSkeleton.h"
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

/* --- 定義 ---- */

void ActorSkeleton::Initialize(
ID3D11Device* device,
const char* name,
const Vector3& position,
const Quaternion& rotation,
const Vector3& scale)
{
	Actor::Initialize<GLTFSkinnedMesh>(
		device,
		name,
		position,
		rotation,
		scale,
		"Enemy",
		"Data/Model/GLTF/Enemy/Skeleton/skeleton_skinned.gltf");

	auto behavior{ CreateBehavior<BehaviorSkeleton>() };
	RegisterAnimation();

	//AddComponent<FaceDirection>(0.1f, true);
	CreateRigidBody(1.0f, true);

	AddComponent<WeaponHandler>();

	AddComponent<Health>(100, 100);
	auto character{ AddComponent<Character>() };
	
	character->SetRadius(0.5f * scale.x);
	character->SetHeight(2.0f * scale.y);

	CreateCollider<ColliderCapsule>("Body", Vector3(0, 0, 0), Vector3(0, character->GetHeight(), 0), character->GetRadius(), Vector4(0, 1, 0, 1));
}

void ActorSkeleton::RegisterAnimation()
{
	auto model_skinned{ GetModel<GLTFSkinnedMesh>() };
	auto animator{ CreateAnimator() };
	animator->RegisterAnimation(Idle, 3);
	animator->RegisterAnimation(Walk, 5);
	animator->RegisterAnimation(Run, 4);
	animator->RegisterAnimation(RunCrawl, 4);
	animator->RegisterAnimation(Scream, 1);
	const auto& anim_dead{ animator->RegisterAnimation(Dead, 2) };
	const auto& anim_attack{ animator->RegisterAnimation(Attack, 0) };

	anim_attack->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			if (auto weapon = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
			{
				weapon->GetComponent<Weapon>()->CollideCapsule(1.0f, 0.25f);
			}
		}, 0.8f);

	anim_attack->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			if (auto weapon = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
			{
				weapon->GetComponent<Weapon>()->NoCollide();
			}
		}, 1.5f);

	anim_attack->SetEventWhenExit(
		[](const std::shared_ptr<Actor>& actor)
		{
			if (auto weapon = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
			{
				weapon->GetComponent<Weapon>()->NoCollide();
			}
		});

	anim_dead->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			GameSys.GetActorManager()->Remove(actor);
			GameSys.GetActorManager()->Remove(actor->GetComponent<WeaponHandler>()->GetWeaponActor());
		}, anim_dead->MeasureAnimationTimeEnd(model_skinned));

}
