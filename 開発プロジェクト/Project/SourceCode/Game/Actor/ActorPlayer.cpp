#include "ActorPlayer.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Input/Input.h"
#include "Input/GamePad.h"

//#include "AudioPlayer.h"
#include "Audio/AudioHandle.h"
#include "../Animation/Animator.h"
//#include "ColliderOld.h"
//#include "ColliderAABB.h"
//#include "ColliderSphere.h"
#include "../Component/Health/Health.h"
#include "../Component/Character.h"
#include "../Behavior/StateMachine/StateMachine.h"
#include "../Behavior/StateMachine/StateMachineStatePlayer.h"
#include "../Component/Weapon/WeaponHandler.h"
#include "../Component/Weapon/Weapon.h"
#include "../Collider/ColliderCapsule.h"
#include "../General/PlayerController.h"
#include "../General/Inventory.h"
#include "../General/Tutorial.h"
#include "../Physics/RigidBody.h"

#include "ActorManager.h"

/* --- 定義 ---- */

void ActorPlayer::Initialize(
	ID3D11Device* device, 
	const Vector3& position, 
	const Quaternion& rotation, 
	const Vector3& scale)
{
	Actor::Initialize<GLTFSkinnedMesh>(
		device,
		"Player",
		position,
		rotation,
		scale,
		"Player",
		"Data/Model/GLTF/Player/Paladin/paladin6.gltf");

	// アニメーション
	RegisterAnimation();

	{
		constexpr uint32_t initial_state{ MovementState };
		constexpr uint32_t initial_sub_state{ _IdleState };

		auto actor{ shared_from_this() };

		auto state_machine{ CreateBehavior<StateMachine>(initial_state, initial_sub_state) };
		state_machine->RegisterState(MovementState, std::make_shared<MovementStatePlayer>(actor));
		state_machine->RegisterSubState(MovementState, _IdleState, std::make_shared<IdleStatePlayer>(actor));
		state_machine->RegisterSubState(MovementState, _RunState, std::make_shared<RunStatePlayer>(actor));
		state_machine->RegisterSubState(MovementState, _PickupStandState, std::make_shared<PickupItemStandStatePlayer>(actor));
		state_machine->RegisterSubState(MovementState, _PickupRunState, std::make_shared<PickupItemRunStatePlayer>(actor));
		state_machine->RegisterState(AttackState, std::make_shared<AttackStatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _AttackCombo0, std::make_shared<ComboAttack0StatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _AttackCombo1, std::make_shared<ComboAttack1StatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _AttackCombo2, std::make_shared<ComboAttack2StatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _AttackSlash,  std::make_shared<SlashAttackStatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _ParryState, std::make_shared<ParryStatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _RollState, std::make_shared<RollStatePlayer>(actor));
		state_machine->RegisterSubState(AttackState, _JumpAttackState, std::make_shared<JumpAttackStatePlayer>(actor));
		state_machine->RegisterState(DamagedState, std::make_shared<DamagedStatePlayer>(actor));
		state_machine->RegisterState(DeathState, std::make_shared<DeathStatePlayer>(actor));
		state_machine->RegisterState(ConsumeState, std::make_shared<ConsumeStatePlayer>(actor));
		state_machine->RegisterState(EnchantState, std::make_shared<EnchantStatePlayer>(actor));

	}

	CreateRigidBody(1.0f, true);

	AddComponent<WeaponHandler>();
	AddComponent<Health>(100, 100);
	auto character{ AddComponent<Character>() };
	character->SetRadius(0.25f);
	character->SetHeight(2.0f);

	CreateCollider<ColliderCapsule>("Body", Vector3(0, 0, 0), Vector3(0, character->GetHeight(), 0), character->GetRadius(), Vector4(0, 1, 0, 1));
}

void ActorPlayer::RegisterAnimation()
{
	auto model_skinned{ GetModel<GLTFSkinnedMesh>() };
	auto animator{ CreateAnimator() };


	const auto& anim_combo0{ animator->RegisterAnimation(AttackCombo0, 0) };
	const auto& anim_combo1{ animator->RegisterAnimation(AttackCombo1, 1) };
	const auto& anim_combo2{ animator->RegisterAnimation(AttackCombo2, 2) };
	const auto& anim_damaged{ animator->RegisterAnimation(Damaged, 3) };
	const auto& anim_dead0{ animator->RegisterAnimation(Dead0, 4) };
	const auto& anim_dead1{ animator->RegisterAnimation(Dead1, 5) };
	const auto& anim_consume{ animator->RegisterAnimation(Consume, 6) };
	const auto& anim_enchant{ animator->RegisterAnimation(Enchant, 7) };
	const auto& anim_idle{ animator->RegisterAnimation(Idle, 8) };
	const auto& anim_jump_attack{ animator->RegisterAnimation(JumpAttack, 9) };
	const auto& anim_parry{ animator->RegisterAnimation(Parry, 10) };
	const auto& anim_pickup_stand{ animator->RegisterAnimation(PickupStand, 11) };
	const auto& anim_pickup_run{ animator->RegisterAnimation(PickupRun, 12) };
	const auto& anim_roll{ animator->RegisterAnimation(Roll, 13) };
	const auto& anim_run{ animator->RegisterAnimation(Run, 14) };
	const auto& anim_slash{ animator->RegisterAnimation(AttackSkill0,16) };
	const auto& anim_walk{ animator->RegisterAnimation(Walk,17) };

	// 待機ステート
	{
		anim_idle->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetRigidBody()->SuddenStop(1.0f);
			});
	}
	// 通常０
	{
		anim_combo0->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (Tutorial::GetInstance().IsTutorial())
				{
					if(Tutorial::GetInstance().GetFlag(Tutorial::ShowAttackText) != Tutorial::AlreadyDisplayed) return;
				}

				if (Input::GetInstance().GetGamePad().GetButton(GamePad::RIGHT_SHOULDER, GamePad::Flag::DOWN))
				{
					auto state_machine{ actor->GetBehavior<StateMachine>() };
					state_machine->SetNextState(ActorPlayer::AttackState);
					state_machine->SetNextSubState(ActorPlayer::_AttackCombo1);
				}
			}, 0.27f, 0.6f);
		anim_combo0->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				auto state_machine{ actor->GetBehavior<StateMachine>() };

				//auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL() };
				//if (lx != 0.0f || ly != 0.0f)
				//{
				//	state_machine->SetNextState(ActorPlayer::MovementState);
				//	state_machine->SetNextSubState(ActorPlayer::_RunState);
				//}

				if (Input::GetInstance().GetGamePad().GetButton(GamePad::LEFT_SHOULDER, GamePad::DOWN))
				{
					state_machine->SetNextState(ActorPlayer::AttackState);
					// ローリング
					state_machine->SetNextSubState(ActorPlayer::_RollState);
					return;
				}


			}, 0.3f, FLT_MAX);
		anim_combo0->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->CollideCombo0();
					AudioHandle attack;
					attack.Play(L"Data/Audio/SE/Mixed/swing.wav", 0.3f);
					attack.ReverbEffect();
				}
			}, 0.1f);
		anim_combo0->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}
			}, 0.3f);

		anim_combo0->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}
				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowAttackText) == Tutorial::Displaying)
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowAttackText, Tutorial::AlreadyDisplayed);
					Tutorial::GetInstance().SetFlag(Tutorial::ShowComboText, Tutorial::Displaying);
				}
			});

	}
	// 通常１
	{
		anim_combo1->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (Input::GetInstance().GetGamePad().GetButton(GamePad::RIGHT_SHOULDER, GamePad::Flag::DOWN))
				{
					auto state_machine{ actor->GetBehavior<StateMachine>() };
					state_machine->SetNextState(ActorPlayer::AttackState);
					state_machine->SetNextSubState(ActorPlayer::_AttackCombo2);
				}
			}, 0.4f, 0.6f);
		anim_combo1->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				//auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL() };
				//if (lx != 0.0f || ly != 0.0f)
				//{
				//	auto state_machine{ actor->GetBehavior<StateMachine>() };
				//	state_machine->SetNextState(ActorPlayer::MovementState);
				//	state_machine->SetNextSubState(ActorPlayer::_RunState);
				//}

				auto state_machine{ actor->GetBehavior<StateMachine>() };
				if (Input::GetInstance().GetGamePad().GetButton(GamePad::LEFT_SHOULDER, GamePad::DOWN))
				{
					state_machine->SetNextState(ActorPlayer::AttackState);
					// ローリング
					state_machine->SetNextSubState(ActorPlayer::_RollState);
					return;
				}

			}, 0.4f, FLT_MAX);
		anim_combo1->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->CollideCombo1();
					AudioHandle attack;
					attack.Play(L"Data/Audio/SE/Mixed/swing.wav", 0.3f);
					attack.ReverbEffect();
				}
			}, 0.1f);
		anim_combo1->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}
			}, 0.3f);
		anim_combo1->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (Input::GetInstance().GetGamePad().GetButton(GamePad::RIGHT_SHOULDER, GamePad::Flag::DOWN))
				{
					actor->GetBehavior<StateMachine>()->SetNextSubState(State::_AttackCombo2);
				}
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}

				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowComboText) == Tutorial::Displaying)
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowComboText, Tutorial::AlreadyDisplayed);
					Tutorial::GetInstance().SetFlag(Tutorial::ShowSlashText, Tutorial::Displaying);
				}
			});
	}
	// 通常２
	{
		anim_combo2->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/mixkit/mixkit-fighting-mans-voice-2171.wav", 0.3f);
				attack.ReverbEffect();
			}
		);

		anim_combo2->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				auto state_machine{ actor->GetBehavior<StateMachine>() };
				if (Input::GetInstance().GetGamePad().GetButton(GamePad::LEFT_SHOULDER, GamePad::DOWN))
				{
					state_machine->SetNextState(ActorPlayer::AttackState);
					// ローリング
					state_machine->SetNextSubState(ActorPlayer::_RollState);
					return;
				}

				//auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL() };

				//if (lx != 0.0f || ly != 0.0f)
				//{
				//	auto state_machine{ actor->GetBehavior<StateMachine>() };
				//	state_machine->SetNextState(ActorPlayer::MovementState);
				//	state_machine->SetNextSubState(ActorPlayer::_RunState);
				//}
			}, 0.6f, FLT_MAX);
		anim_combo2->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->CollideCombo2();

					AudioHandle attack;
					attack.Play(L"Data/Audio/SE/Mixed/swing.wav", 0.3f);
					attack.ReverbEffect();
				}
			}, 0.1f);
		anim_combo2->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}
			}, 0.4f);
		anim_combo2->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}

				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowComboText) == Tutorial::Displaying)
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowComboText, Tutorial::AlreadyDisplayed);
					Tutorial::GetInstance().SetFlag(Tutorial::ShowSlashText, Tutorial::Displaying);
				}
			});
	}
	// 特殊０
	{
		anim_slash->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/mixkit/mixkit-fighting-mans-voice-2171.wav", 0.3f);
				attack.ReverbEffect();
			}
		);

		anim_slash->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					auto weapon{ weapon_actor->GetComponent<Weapon>() };

					
					AudioHandle attack;
					attack.Play(L"Data/Audio/SE/Mixed/swing_slash.wav", 0.3f);
					attack.ReverbEffect();

					if (const auto& weapon_enchant = weapon->GetWeaponEnchant())
					{
						weapon_enchant->PlaySlashTrail(actor);
						
						//if (efk)
						//{
						//	efk->SetScale({ 0.5f, 0.5f, 0.5f });
						//	efk->SetRotation(actor->GetWorldRotation());
						//}
					}
				}
			}, 0.25f);
		anim_slash->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					Vector3 f{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

					weapon_actor->GetComponent<Weapon>()->CollideSlash();
				}
			}, 0.3f);
		anim_slash->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetBehavior<StateMachine>()->SetTransitionFlag(true);
				
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					auto weapon{ weapon_actor->GetComponent<Weapon>() };
					weapon_actor->GetComponent<Weapon>()->NoCollide();
					// スキルを使用したらエンチャント効果は消える
					//weapon->SetWeaponEnchant(nullptr);
				}
			}, 0.6f);
		anim_slash->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowSlashText) == Tutorial::Displaying)
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowSlashText, Tutorial::AlreadyDisplayed);
					Tutorial::GetInstance().SetFlag(Tutorial::ShowDodgeText, Tutorial::Displaying);
				}
			});
	}
	// ローリング
	{
		anim_roll->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(10.0f);
				actor->GetCollider<ColliderCapsule>("Body")->SetIntersectOnly(true);
				actor->GetRigidBody()->SetDynamicFriction(1.0f);

				AudioHandle se;
				se.Play(L"Data/Audio/SE/Mixed/dodge.wav", 0.2f);
				se.ReverbEffect();
			});

		anim_roll->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				const auto& attackers{ actor->GetComponent<Health>()->GetAttackers() };

				if (!attackers.empty())
				{
					GameSys.SetTimeLapseDelta(0.5f, 5.0f);
					GameSys.GetPlayerController()->SetTransitionJumpAttack(true);
					GameSys.GetPlayerController()->SelectJumpAttackTarget(attackers);

					AudioHandle attack;
					attack.Play(L"Data/Audio/SE/pixabay/bass-dropmp3-14596.wav", 0.5f);
					//attack.ReverbEffect();

					if (Tutorial::GetInstance().GetFlag(Tutorial::ShowDodgeText) == Tutorial::Displaying)
					{
						Tutorial::GetInstance().SetFlag(Tutorial::ShowDodgeText, Tutorial::AlreadyDisplayed);
						Tutorial::GetInstance().SetFlag(Tutorial::ShowCounterText, Tutorial::Displaying);
					}
				}

			}, 0.0f, anim_roll->MeasureAnimationTimeEnd(model_skinned));

		anim_roll->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(0.0f);
				GameSys.GetPlayerController()->SetTransitionJumpAttack(false);
				GameSys.SetTimeLapseDelta(1.0f, 10.0f);
				actor->GetCollider<ColliderCapsule>("Body")->SetIntersectOnly(false);
				actor->GetRigidBody()->SetDynamicFriction();
			});
	}
	// ジャンプ攻撃
	{
		anim_jump_attack->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(10.0f);
				actor->GetComponent<WeaponHandler>()->SetDuraRedReflection(0.0f);
				GameSys.SetTimeLapseRatio(1.0f);
				GameSys.SetTimeLapseDelta(0.2f, 2.0f);

				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/mixkit/mixkit-fighting-mans-voice-2171.wav", 0.3f);
				attack.ReverbEffect();

				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowCounterText) == Tutorial::Displaying)
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowCounterText, Tutorial::AlreadyDisplayed);
				}

			});
		anim_jump_attack->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				
				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/Mixed/swing_jump_attack.wav", 0.3f);
				attack.ReverbEffect();

				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->CollideJumpATK();
				}
				GameSys.SetTimeLapseDelta(1.0f, 10.0f);
			}, 0.6f);
		anim_jump_attack->AddEventOncePerLoop(
			[](const std::shared_ptr<Actor>& actor)
			{
				if (auto weapon_actor = actor->GetComponent<WeaponHandler>()->GetWeaponActor())
				{
					weapon_actor->GetComponent<Weapon>()->NoCollide();
				}
			}, 1.0f);
		anim_jump_attack->SetEventWhenExit(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(0.0f);
				actor->GetComponent<WeaponHandler>()->SetDuraRedReflection(1.0f);
				GameSys.GetPlayerController()->ClearJumpAttackTarget();
			});
	}
	// 死亡０
	{
		anim_dead0->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(100.0f);
			});
	}
	// 死亡１(吹っ飛ぶ)
	{
		anim_dead1->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				actor->GetComponent<Health>()->SetInvincibleTimer(100.0f);
			});
	}
	// アイテム使用
	{
		anim_consume->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				EffekseerHandle heal_efk("Heal/heal.efk", actor->GetWorldPosition());
				GameSys.GetInventory()->ConsumeSelectItem(actor);
			});
	}
	// エンチャント
	{
		anim_enchant->SetEventWhenEnter(
			[](const std::shared_ptr<Actor>& actor)
			{
				GameSys.GetInventory()->EnchantSelectItem(actor);
			});
	}

	{
		anim_damaged->AddEventPerFrame(
			[](const std::shared_ptr<Actor>& actor)
			{
				auto state_machine{ actor->GetBehavior<StateMachine>() };
				if (Input::GetInstance().GetGamePad().GetButton(GamePad::LEFT_SHOULDER, GamePad::DOWN))
				{
					state_machine->SetNextState(ActorPlayer::AttackState);
					// ローリング
					state_machine->SetNextSubState(ActorPlayer::_RollState);
					return;
				}

		//auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL() };

		//if (lx != 0.0f || ly != 0.0f)
		//{
		//	auto state_machine{ actor->GetBehavior<StateMachine>() };
		//	state_machine->SetNextState(ActorPlayer::MovementState);
		//	state_machine->SetNextSubState(ActorPlayer::_RunState);
		//}
			}, 0.35f, FLT_MAX);
	}
}


