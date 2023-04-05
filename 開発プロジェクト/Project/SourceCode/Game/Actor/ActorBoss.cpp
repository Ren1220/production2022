#include "ActorBoss.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "General/AtaiEngine.h"

#include "Graphics/Model/GLTFSkinnedMesh.h"
#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Camera/Shake/CameraShakeDamaged.h"

#include "Game/Actor/Actor.h"
#include "Game/Animation/Animator.h"
#include "Game/Animation/Animation.h"
#include "Game/Behavior/BehaviorTree/Enemy/BehaviorBoss.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/Component/Light/PointLight.h"
#include "Game/Component/Projectile/ProjectileRock.h"
#include "Game/Component/Effect/Impact.h"
#include "Game/Collider/ColliderCapsule.h"
#include "Game/Collider/ColliderSphere.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/General/BattleSystem.h"
#include "Game/Stage/StageManager.h"
#include "Game/HUD/HUDManager.h"
#include "Game/Physics/RigidBody.h"


/* --- 定義 ---- */

void ActorBoss::Initialize(
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
		scale * 1.5f,
		"Boss",
		"Data/Model/GLTF/Enemy/Boss/boss2.gltf");

	CreateRigidBody(3.0f, true);

	CreateBehavior<BehaviorBoss>();
	AddComponent<Health>(1000, 1000);
	auto character{ AddComponent<Character>() };
	character->SetRadius(1.5f * scale.x);
	character->SetHeight(2.0f * scale.y);

	//{
	//	auto light = CreateChildActor<Actor>();

	//	light->Initialize(
	//		"LightBoss",
	//		{ 0.0f, 4.0f, 0.0f },
	//		Quaternion::Identity(),
	//		{ 0, 0, 0 },
	//		"LightBoss");

	//	light->AddComponent<PointLight>(Vector3(0.0f, 2.0f, 0.0f), 10.0f, Vector3(1.0f, 1.0f, 1.0f), 7.0f, true);
	//}

	CreateCollider<ColliderCapsule>("Body", Vector3(0, 0, 0), Vector3(0, character->GetHeight(), 0), character->GetRadius(), Vector4(0, 1, 0, 1));

	// 攻撃に使われる当たり判定
	auto col_hand{ CreateCollider<ColliderSphere>("Hand", Vector3(0, 0, 0), 1.0f, Vector4(1, 1, 0, 1), true) };
	col_hand->SetActiveFlag(false); // 切っておく

	auto col_tackle{ CreateCollider<ColliderSphere>("Tackle", Vector3(0, 1, 0), 2.0f, Vector4(1, 1, 0, 1), true) };
	col_tackle->SetActiveFlag(false); 

	auto col_jump_atk{ CreateCollider<ColliderSphere>("JumpATK", Vector3(0, 0, 0), 3.0f, Vector4(1, 1, 0, 1), true) };
	col_jump_atk->SetActiveFlag(false);

	auto col_roaring{ CreateCollider<ColliderSphere>("Roaring", Vector3(0, 0, 0), 5.0f, Vector4(1, 1, 0, 1), true) };
	col_roaring->SetActiveFlag(false);

	RegisterAnimation();
}

void ActorBoss::RegisterAnimation()
{
	auto model_skinned{ GetModel<GLTFSkinnedMesh>() };

	auto animator{ CreateAnimator() };

	animator->RegisterAnimation(Damaged,	0);
	const auto& anim_dead{ animator->RegisterAnimation(Dead,		1) };
	animator->RegisterAnimation(Idle,		2);
	animator->RegisterAnimation(Jump,		3);
	const auto& anim_backjump{ animator->RegisterAnimation(BackJump,	3) };
	const auto& anim_jump_atk{ animator->RegisterAnimation(JumpAttack,	4) };
	const auto& anim_kick	 { animator->RegisterAnimation(Kick,		5) };
	animator->RegisterAnimation(Attack0,	6);
	const auto& anim_roaring{ animator->RegisterAnimation(Roaring,	7) };
	const auto& anim_roaring_b{ animator->RegisterAnimation(RoaringBegin,	7) };
	animator->RegisterAnimation(Run,		8);
	const auto& anim_attack1{ animator->RegisterAnimation(Attack1,	9) };
	const auto& anim_tackle{ animator->RegisterAnimation(Tackle, 10) };
	animator->RegisterAnimation(Walk,		11);
	const auto& anim_return{ animator->RegisterAnimation(Return, 3) };

	// 攻撃1
	{
		const auto& col_hand{ GetCollider<ColliderSphere>("Hand") };
		col_hand->SetReferenceNode("mixamorig:LeftHand");
		col_hand->AddOnHitEventCallBack("Attack1",
			[](const std::shared_ptr<Actor>& actor, const std::shared_ptr<Actor>& opponent)
			{
				if (opponent->CompareGroupName("Player"))
				{
					BattleSystem::GetInstance().SendCombatData(actor, opponent, 20, 1.0f);
				}
			});

		anim_attack1->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Hand")->SetActiveFlag(true);

			}, 1.2f);
		anim_attack1->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Hand")->SetActiveFlag(false);

			}, 1.5f);
	}
	// タックル
	{
		const auto& col_tackle{ GetCollider<ColliderSphere>("Tackle") };
		col_tackle->AddOnHitEventCallBack("Tackle",
			[](const std::shared_ptr<Actor>& actor, const std::shared_ptr<Actor>& opponent)
			{
				if (opponent->CompareGroupName("Player"))
				{
					BattleSystem::GetInstance().SendCombatData(actor, opponent, 20, 1.0f);

					// TODO プレイヤーにノックバックステートを追加する
					//Vector3 vec{ (opponent->GetWorldPosition() - actor->GetWorldPosition()).Normalize() };
					//vec *= 10.0f;
					//vec.y += 1.0f;
					//opponent->GetRigidBody()->AddVelocity(vec);
				}
			});

		anim_tackle->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Tackle")->SetActiveFlag(true);

				actor->GetRigidBody()->AddVelocity(Vector3(Matrix::CreateRotation(actor->GetWorldRotation()).m[2]) * 2.0f);
			}, 0.3f, 0.7f);
		anim_tackle->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetRigidBody()->AddVelocity(Vector3(Matrix::CreateRotation(actor->GetWorldRotation()).m[2]) * 5.0f);
				actor->GetRigidBody()->SetDynamicFriction(3.0f);
			}, 0.7f);
		anim_tackle->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Tackle")->SetActiveFlag(false);
				actor->GetRigidBody()->SetDynamicFriction();
			}, 1.3f);
	}
	// 遠距離攻撃 //0.55 keck
	{
		anim_kick->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				Vector3 pos{ actor->GetWorldPosition() };
				pos.y += 1.0f;

				// 球を生成する
				auto prj{ GameSys.GetActorManager()->Create<Actor>() };
				prj->Initialize(
					"Projectile",
					pos,
					{ 0, 1, 0, 0 },
					{ 0.5f,0.5f,0.5f },
					"Projectile");

				prj->CreateRigidBody(1.0f, false);
				prj->CreateCollider<ColliderSphere>("Body", Vector3(0, 0, 0), 1.0f, Vector4(1, 0, 0, 1), true);

				Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

				prj->AddComponent<ProjectileRock>(actor, 10, f, 4.0f, 20.0f);
			}, 0.55f);
	}
	// 咆哮
	{
		const auto& col_roaring{ GetCollider<ColliderSphere>("Roaring") };
		col_roaring->AddOnHitEventCallBack("Roaring",
			[](const std::shared_ptr<Actor>& actor, const std::shared_ptr<Actor>& opponent)
			{
				if (opponent->CompareGroupName("Player"))
				{
					Vector3 vec{ (opponent->GetWorldPosition() - actor->GetWorldPosition()) };
					opponent->GetRigidBody()->AddVelocity(vec.Normalize() * (std::max)(0.0f, (7.0f - vec.Length()) * 0.2f));
				}
			});

		anim_roaring->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Roaring")->SetActiveFlag(true);
				if (auto camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera())
				{
					camera->SetCameraShake(std::make_unique<CameraShakeDamaged>(3.5f, 0.03f, 100.0f));
				}
			}, 1.5f);

		anim_roaring->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Roaring")->SetActiveFlag(false);
			}, 5.0f);
	}
	// 咆哮＿開始時
	{
		anim_roaring_b->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Roaring")->SetActiveFlag(true);
				if (auto camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera())
				{
					camera->SetCameraShake(std::make_unique<CameraShakeDamaged>(3.5f, 0.03f, 100.0f));
				}
			}, 1.5f);

		anim_roaring_b->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("Roaring")->SetActiveFlag(false);
			}, 5.0f);

		anim_roaring_b->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				GameSys.GetHUDManager()->GetHealth().SetShowBossHealthFlag(true);
			});
	}
	// ジャンプ攻撃 
	//0.5 1.7jatk
	{
		// コライダーに衝突したときに実行する関数
		const auto& col_jump_atk{ GetCollider<ColliderSphere>("JumpATK") };
		col_jump_atk->AddOnHitEventCallBack("JumpATK",
			[](const std::shared_ptr<Actor>& actor, const std::shared_ptr<Actor>& opponent)
			{
				BattleSystem::GetInstance().SendCombatData(actor, opponent, 30, 1.0f);
				//Vector3 vec{ (opponent->GetWorldPosition() - actor->GetWorldPosition()).Normalize() };
				//vec *= 5.0f;
				//vec.y += 1.0f;
				//opponent->GetRigidBody()->AddVelocity(vec);
			});

		anim_jump_atk->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
				Vector3 vec{ player_pos - actor->GetWorldPosition() };
				actor->SetWorldPosition(actor->GetWorldPosition() + Vector3(0, 0.1f, 0));
				actor->GetRigidBody()->AddVelocity(vec.Normalize() * Vector3(6.0f, 0.0f, 6.0f) + Vector3(0, 4.0f, 0));
			}, 0.5f);
		anim_jump_atk->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("JumpATK")->SetActiveFlag(true);
				
				auto impact{ GameSys.GetActorManager()->Create<Actor>() };
				impact->Initialize<GLTFStaticMesh>(
					MySystem.GetDevice(),
					"Impact",
					{ actor->GetWorldPosition().x, -0.1f, actor->GetWorldPosition().z },
					Quaternion::Identity(),
					{ 2.0f, 2.0f, 2.0f },
					"Effect",
					"Data/Model/GLTF/Floor/floor_impact.gltf");
				impact->AddComponent<Impact>(1.0f);

			}, 1.7f);
		anim_jump_atk->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetCollider<ColliderSphere>("JumpATK")->SetActiveFlag(false);
			}, 1.8f);
	}
	anim_backjump->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			const Vector3& init_pos{ actor->GetBehavior<BehaviorBoss>()->GetInitPos() };
			Vector3 vec_init{ init_pos - actor->GetWorldPosition() };
			if (vec_init.Length() > 8.0f)
			{
				actor->SetWorldPosition(actor->GetWorldPosition() + Vector3(0, 0.1f, 0));
				actor->GetRigidBody()->AddVelocity(vec_init.Normalize()* Vector3(8.0f, 0.0f, 8.0f) + Vector3(0, 4.0f, 0));
			}
			else
			{
				const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
				Vector3 vec{ player_pos - actor->GetWorldPosition() };
				actor->SetWorldPosition(actor->GetWorldPosition() + Vector3(0, 0.1f, 0));
				actor->GetRigidBody()->AddVelocity(vec.Normalize() * Vector3(-5.0f, 0.0f, -5.0f) + Vector3(0, 4.0f, 0));
			}

		}, 1.2f);

	anim_return->AddEventOncePerLoop(
		[](const std::shared_ptr<Actor>& actor)
		{
			const Vector3& init_pos{ actor->GetBehavior<BehaviorBoss>()->GetInitPos() };
			Vector3 vec{ init_pos - actor->GetWorldPosition() };
			actor->SetWorldPosition(actor->GetWorldPosition() + Vector3(0, 0.1f, 0));
			float len{ vec.Length() };
			actor->GetRigidBody()->AddVelocity(vec.Normalize() * Vector3(len * 0.5f, 0.0f, len * 0.5f) + Vector3(0, len * 0.75f, 0));
		}, 1.2f);

	// 死亡
	{
		anim_dead->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				GameSys.GetActorManager()->Remove(actor);
			}, anim_dead->MeasureAnimationTimeEnd(model_skinned));

		anim_dead->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				GameSys.GetHUDManager()->GetHealth().SetShowBossHealthFlag(true);
				GameSys.GetStageManager()->ActivateWarp();
			});
	}

}
