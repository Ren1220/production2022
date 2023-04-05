#include "ActorSpider.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "General/AtaiEngine.h"
#include "Graphics/Model/GLTFStaticMesh.h"

#include "../Actor/ActorPlayer.h"
#include "../Animation/Animator.h"
#include "../Physics/RigidBody.h"
#include "../Behavior/BehaviorTree/Enemy/BehaviorSpider.h"
#include "../Behavior/StateMachine/StateMachine.h"
#include "../Behavior/StateMachine/StateMachineStatePlayer.h"
#include "../Collider/ColliderCapsule.h"
//#include "FaceDirection.h"
#include "../Component/Health/Health.h"
#include "../Component/Character.h"	
#include "../Component/Weapon/Weapon.h"
#include "../Component/Weapon/WeaponHandler.h"
#include "../Component/Projectile/ProjectileSpiderPoison.h"
#include "../Collider/ColliderManager.h"
#include "../Collider/ColliderSphere.h"
#include "../General/GameSystem.h"
#include "../General/BattleSystem.h"

/* --- 定義 ---- */

void ActorSpider::Initialize(
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
		scale * 0.2f,
		"Enemy",
		"Data/Model/GLTF/Enemy/Spider/spider4.gltf");

	auto behavior{ CreateBehavior<BehaviorSpider>() };
	RegisterAnimation();
	//AddComponent<FaceDirection>(0.1f, true);
	CreateRigidBody(1.0f, true);

	AddComponent<WeaponHandler>();

	AddComponent<Health>(100, 100);
	auto character{ AddComponent<Character>() };
	character->SetRadius(0.5f * scale.x);
	character->SetHeight(2.0f * scale.y); // 少し高め

	CreateCollider<ColliderCapsule>("Body", Vector3(0, 0, 0), Vector3(0, character->GetHeight(), 0), character->GetRadius(), Vector4(0, 1, 0, 1), false);
}

void ActorSpider::RegisterAnimation()
{
	auto model_skinned{ GetModel<GLTFSkinnedMesh>() };
	auto animator{ CreateAnimator() };
	const auto& anim_dead { animator->RegisterAnimation(Dead, 1) };
	const auto& anim_dead2{ animator->RegisterAnimation(Dead2, 2) };
	animator->RegisterAnimation(Walk, 7);
	auto anim_jump{ animator->RegisterAnimation(Jump, 4) };
	auto anim_bite{ animator->RegisterAnimation(Bite, 0) };
	auto anim_launch{ animator->RegisterAnimation(Launch, 5) };

	anim_bite->AddEventPerFrame(
		[](const std::shared_ptr<Actor>& actor)
		{
			Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };
			Vector3 p{ actor->GetWorldPosition() };
			p += f;

			auto hit_player{ GameSys.GetColliderManager()->GetCollidedActorsWithSphere(p, 0.5f, "Player") };

			for (const auto& p : hit_player)
			{
				BattleSystem::GetInstance().SendCombatData(actor, p, 10, 1.0f, 0.5f);
			}
			
		}, 0.4f, 0.6f);

	anim_jump->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };
			actor->GetRigidBody()->AddForce((f + Vector3(0, 0.2f, 0)) * 400.0f);
		}, 0.5f);

	anim_launch->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			Vector3 pos{ actor->GetWorldPosition() };
			pos.y += 1.0f;

			// 球を生成する
			auto prj{ GameSys.GetActorManager()->Create<Actor>() };
			prj->Initialize<GLTFStaticMesh>(
				MySystem.GetDevice(),
				"Projectile",
				pos,
				{ 0, 1, 0, 0 },
				{ 0.5f,0.5f,0.5f },
				"Projectile",
				"Data/Model/GLTF/Enemy/Spider/projectile.gltf");

			prj->CreateRigidBody(1.0f, false);
			prj->CreateCollider<ColliderSphere>("Body", Vector3(0, 0, 0), 0.25f, Vector4(1, 0, 0, 1), true);

			Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

			prj->AddComponent<ProjectileSpiderPoison>(actor, 10, f, 4.0f, 20.0f);
		}, 1.8f);

	anim_dead->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			GameSys.GetActorManager()->Remove(actor);
		}, anim_dead->MeasureAnimationTimeEnd(model_skinned));
	anim_dead2->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			GameSys.GetActorManager()->Remove(actor);
		}, anim_dead->MeasureAnimationTimeEnd(model_skinned));

}
