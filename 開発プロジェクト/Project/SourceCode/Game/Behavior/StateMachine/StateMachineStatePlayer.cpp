#include "StateMachineStatePlayer.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Input/Input.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Audio/AudioHandle.h"

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorPlayer.h"
#include "Game/Animation/Animator.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/General/Inventory.h"
#include "Game/General/Title.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/Component/Weapon/Weapon.h"
#include "Game/Component/Weapon/WeaponHandler.h"
#include "Game/Component/Item/Item.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/Physics/RigidBody.h"
#include "StateMachine.h"
//#include "ColliderOld.h"
//#include "CollideHandler.h"
//#include "FrameCamera.h"
//#include "Collision.h"
//#include "OnHitEvents.h"
//#include "Component/Camera/Camera.h"


/* --- ��` ---- */

void MovementStatePlayer::Start()
{
}

void MovementStatePlayer::Execute(float elapsed_time)
{
	const auto& gamepad{ Input::GetInstance().GetGamePad() };
	auto actor{ GetActor() };

	// �^�C�g���ł͓����Ȃ�
	if (TitleSystem::GetInstance().IsTitle()) return;

	if (GetActor()->GetComponent<Health>()->IsDamaged())
	{
		// �_���[�W���󂯂��ꍇ��_���[�W�X�e�[�g�ɑJ��
		GetStateMachine()->SetNextState(ActorPlayer::DamagedState);
		return;
	}

	if (gamepad.GetButton(GamePad::DPAD_UP, GamePad::DOWN))
	{
		// �g�p�\�ȃA�C�e����I�����Ă���ꍇ
		if (auto select_item = GameSys.GetInventory()->GetSelectItem())
		{
			if (select_item->GetComponent<Item>()->CanConsume(actor))
			{
				GetStateMachine()->SetNextState(ActorPlayer::ConsumeState);
				return;
			}
		}
	}

	if (gamepad.GetButton(GamePad::DPAD_DOWN, GamePad::DOWN))
	{
		// �G���`�����g�\�ȃA�C�e����I�����Ă���ꍇ
		if (auto select_item = GameSys.GetInventory()->GetSelectItem())
		{
			if (select_item->GetComponent<Item>()->CanEnchant(actor))
			{
				GetStateMachine()->SetNextState(ActorPlayer::EnchantState);
				return;
			}
		}
	}

	if (gamepad.GetButton(GamePad::RIGHT_SHOULDER, GamePad::DOWN))
	{
		GetStateMachine()->SetNextState(ActorPlayer::AttackState);
		// �ߐڍU���X�e�[�g�ɑJ��
		GetStateMachine()->SetNextSubState(ActorPlayer::_AttackCombo0);
		return;
	}

	if (gamepad.GetTriggerR(GamePad::DOWN))
	{
		GetStateMachine()->SetNextState(ActorPlayer::AttackState);
		// �ߐړ���U���X�e�[�g�ɑJ��
		GetStateMachine()->SetNextSubState(ActorPlayer::_AttackSlash);
		return;
	}

	if (gamepad.GetButton(GamePad::LEFT_SHOULDER, GamePad::DOWN))
	{
		GetStateMachine()->SetNextState(ActorPlayer::AttackState);
		// ���[�����O
		GetStateMachine()->SetNextSubState(ActorPlayer::_RollState);
		return;
	}

	// �q�X�e�[�g�̎��s�֐�
	ExecuteSubState(elapsed_time);
}

void MovementStatePlayer::End()
{
}

void RunStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Run, true, 10.0f);
}

void RunStatePlayer::Execute(float elapsed_time)
{
	const auto& gamepad{ Input::GetInstance().GetGamePad() };

	auto actor{ GetActor() };
	auto character{ actor->GetComponent<Character>() };
	auto rigid_body{ actor->GetRigidBody() };

	static float footstep_interval_timer{ 0.0f };

	footstep_interval_timer -= elapsed_time;
	if (footstep_interval_timer < 0.0f)
	{
		AudioHandle footstep;
		footstep.Play(L"./Data/Audio/SE/������/������ ���ʉ� ����01.wav", 0.5f, false, false);
		footstep_interval_timer = 0.333f;
	}


	// ����
	{
		auto camera{ Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera() };

		const auto& forward{ camera->GetForward() };
		const auto& right{ camera->GetRight() };

		Vector3 move_direction{ };

		auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL()};

		if (ly > 0 || ly < 0)
		{
			move_direction += Vector3(forward.x, 0, forward.z).Normalize() * ly;
		}
		if (lx > 0 || lx < 0)
		{
			move_direction += Vector3(right.x, 0, right.z).Normalize() * lx;
		}

		// �ړ�����
		character->Move(move_direction, 10.0f);

		// ���x���O�ɋ߂�
		if (rigid_body->GetVelocity().LengthSquare() < 0.01f)
		{
			// �ҋ@�X�e�[�g�ɑJ��
			GetStateMachine()->SetNextSubState(ActorPlayer::State::_IdleState);
			return;
		};
	}


	//// ������̂Ă�
	//if (gamepad.GetButton(GamePad::Y, GamePad::DOWN))
	//{
	//	// ������t�B�[���h�A�C�e����
	//	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
	//	{
	//		weapon_handler->ChangeWeapon(nullptr);
	//	}
	//}

	// �A�C�e�����E��
	if (gamepad.GetButton(GamePad::X, GamePad::DOWN))
	{
		// �C���x���g���ɋ󂫂����邩
		if (GameSystem::GetInstance().GetInventory()->IsInventorySpace())
		{
			auto& collider_manager{ GameSystem::GetInstance().GetColliderManager() };

			float item_pickup_range{ GameSystem::GetInstance().GetPlayerController()->GetItemPickupRange() };

			// �͈͓��ɃA�C�e�������݂��Ă��邩
			auto collided_item_actors{ collider_manager->GetCollidedActorsWithSphere(actor->GetWorldPosition(), item_pickup_range, "Item") };


			for (const auto& collided_item_actor : collided_item_actors)
			{
				// �C���x���g���ɃA�C�e����ǉ�
				GameSystem::GetInstance().GetInventory()->AddItem(collided_item_actor);
				// ��������A�C�e�����t�B�[���h�ォ��폜
				GameSystem::GetInstance().GetActorManager()->Remove(collided_item_actor);
				// �A�C�e������X�e�[�g�ɑJ��
				GetStateMachine()->SetNextSubState(ActorPlayer::_PickupRunState);
				return;
			}
		}

		//// �͈͓��ɕ��킪���݂��Ă��邩
		//auto collided_weapon_actors{ collider_manager->GetCollidedActorsWithSphere(actor->GetWorldPosition(), item_pickup_range, "FieldWeapon") };

		//for (const auto& collided_weapon_actor : collided_weapon_actors)
		//{
		//	// ������t�B�[���h�A�C�e����
		//	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
		//	{
		//		weapon_handler->ChangeWeapon(collided_weapon_actor);
		//		// �A�C�e������X�e�[�g�ɑJ��
		//		GetStateMachine()->SetNextSubState(ActorPlayer::_PickupRunState);
		//	}
		//	return;
		//}
	}

}

void RunStatePlayer::End()
{
	//StopAnimation();
}

void IdleStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Idle, true);
}

void IdleStatePlayer::Execute(float elapsed_time)
{
	const auto& gamepad{ Input::GetInstance().GetGamePad() };

	auto actor = GetActor();

	auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL()};

	if (lx > 0 || lx < 0 || ly < 0 || ly > 0)
	{
		// ����X�e�[�g�ɑJ��
		GetStateMachine()->SetNextSubState(ActorPlayer::_RunState);
		return;
	}

	//// ������̂Ă�
	//if (gamepad.GetButton(GamePad::Y, GamePad::DOWN))
	//{
	//	// ������t�B�[���h�A�C�e����
	//	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
	//	{
	//		weapon_handler->ChangeWeapon(nullptr);
	//	}
	//}

	// �A�C�e�����E��
	if (gamepad.GetButton(GamePad::X, GamePad::DOWN))
	{
		// �C���x���g���ɋ󂫂����邩
		if (GameSystem::GetInstance().GetInventory()->IsInventorySpace())
		{
			auto& collider_manager{ GameSystem::GetInstance().GetColliderManager() };

			float item_pickup_range{ GameSystem::GetInstance().GetPlayerController()->GetItemPickupRange() };

			// �͈͓��ɃA�C�e�������݂��Ă��邩
			auto collided_item_actors{ collider_manager->GetCollidedActorsWithSphere(actor->GetWorldPosition(), item_pickup_range, "Item") };

			for (const auto& collided_item_actor : collided_item_actors)
			{
				// �C���x���g���ɃA�C�e����ǉ�
				GameSystem::GetInstance().GetInventory()->AddItem(collided_item_actor);
				// ��������A�C�e�����t�B�[���h�ォ��폜
				GameSystem::GetInstance().GetActorManager()->Remove(collided_item_actor);
				// �A�C�e������X�e�[�g�ɑJ��
				GetStateMachine()->SetNextSubState(ActorPlayer::_PickupStandState);
				return;
			}
		}

		//// �͈͓��ɕ��킪���݂��Ă��邩
		//auto collided_weapon_actors{ collider_manager->GetCollidedActorsWithSphere(actor->GetWorldPosition(), item_pickup_range, "FieldWeapon") };

		//for (const auto& collided_weapon_actor : collided_weapon_actors)
		//{
		//	// ������t�B�[���h�A�C�e����
		//	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
		//	{
		//		weapon_handler->ChangeWeapon(collided_weapon_actor);
		//		// �A�C�e������X�e�[�g�ɑJ��
		//		GetStateMachine()->SetNextSubState(ActorPlayer::_PickupStandState);
		//	}
		//	return;
		//}

	}
}

void IdleStatePlayer::End()
{
	//StopAnimation();
}

void PickupItemStandStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::PickupStand, false, 10.0f);
}

void PickupItemStandStatePlayer::Execute(float elapsed_time)
{
	// �A�j���[�V�����I��
	{
		auto animator = GetActor()->GetAnimator();

		if (animator->HasPlayedAnimation())
		{
			// �ҋ@�X�e�[�g�ɑJ��
			GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
			return;
		}
	}
}

void PickupItemStandStatePlayer::End()
{
}

void PickupItemRunStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::PickupRun, false, 10.0f);
}

void PickupItemRunStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		// ����X�e�[�g�ɑJ��
		GetStateMachine()->SetNextSubState(ActorPlayer::_RunState);
		return;
	}


	// ����
	{
		auto character{ actor->GetComponent<Character>() };
		auto rigid_body{ actor->GetRigidBody() };
		auto camera{ Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera() };

		const auto& forward{ camera->GetForward() };
		const auto& right{ camera->GetRight() };

		Vector3 move_direction{ };

		auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL()};

		if (ly > 0 || ly < 0)
		{
			move_direction += Vector3(forward.x, 0, forward.z).Normalize() * ly;
		}
		if (lx > 0 || lx < 0)
		{
			move_direction += Vector3(right.x, 0, right.z).Normalize() * lx;
		}

		// �ړ�����
		character->Move(move_direction, 10.0f);

		// ���x���O�ɋ߂�
		if (rigid_body->GetVelocity().LengthSquare() < 0.01f)
		{
			// �ҋ@�X�e�[�g�ɑJ��
			GetStateMachine()->SetNextSubState(ActorPlayer::State::_IdleState);
			return;
		};
	}
}

void PickupItemRunStatePlayer::End()
{
}

void AttackStatePlayer::Start()
{
}

void AttackStatePlayer::Execute(float elapsed_time)
{
	if (GetActor()->GetComponent<Health>()->IsDamaged())
	{
		GetStateMachine()->SetNextState(ActorPlayer::DamagedState);
		return;
	}

	// �q�X�e�[�g�̎��s����
	ExecuteSubState(elapsed_time);
}

void AttackStatePlayer::End()
{
	//StopAnimation();
}

void ComboAttack0StatePlayer::Start()
{
	GetActor()->GetRigidBody()->SuddenStop(1.0f);
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::AttackCombo0, false);
}

void ComboAttack0StatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}

	//if (actor->GetComponent<Player>()->CanTransitionNextState())
	//{
	//	const auto& gamepad{ Input::GetInstance().GetGamePad() };
	//	if (gamepad.GetButton(GamePad::RIGHT_SHOULDER, GamePad::DOWN))
	//	{
	//		GetStateMachine()->SetNextSubState(ActorPlayer::_AttackCombo1);
	//		return;
	//	}
	//}
}

void ComboAttack0StatePlayer::End()
{
}

void ComboAttack1StatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::AttackCombo1, false);
}

void ComboAttack1StatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}

	//if (actor->GetComponent<Player>()->CanTransitionNextState())
	//{
	//	const auto& gamepad{ Input::GetInstance().GetGamePad() };
	//	if (gamepad.GetButton(GamePad::RIGHT_SHOULDER, GamePad::DOWN))
	//	{
	//		GetStateMachine()->SetNextSubState(ActorPlayer::_AttackCombo2);
	//		return;
	//	}
	//}
}

void ComboAttack1StatePlayer::End()
{
}

void ComboAttack2StatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::AttackCombo2, false);
}

void ComboAttack2StatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void ComboAttack2StatePlayer::End()
{
}

void DamagedStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Damaged, false, 10.0f);
}

void DamagedStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };
	auto health{ actor->GetComponent<Health>() };
	auto state_machine{ GetStateMachine() };

	if (health->IsDead())
	{
		state_machine->SetNextState(ActorPlayer::DeathState);
		return;
	}

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		state_machine->SetNextState(ActorPlayer::MovementState);
		state_machine->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void DamagedStatePlayer::End()
{
}

void DeathStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Dead1, false);
}

void DeathStatePlayer::Execute(float elapsed_time)
{
}

void DeathStatePlayer::End()
{
}

void ParryStatePlayer::Start()
{
	// TODO : ���G���Ԃ�ݒ肷��i���j
	GetActor()->GetComponent<Health>()->SetInvincibleTimer(0.3f);
}

void ParryStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void ParryStatePlayer::End()
{
}

void SlashAttackStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::AttackSkill0, false);
}

void SlashAttackStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation() || actor->GetBehavior<StateMachine>()->CanTransition())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		actor->GetBehavior<StateMachine>()->SetTransitionFlag(false);
		return;
	}
}

void SlashAttackStatePlayer::End()
{
}

void ConsumeStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Consume, false);
}

void ConsumeStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void ConsumeStatePlayer::End()
{
}

void EnchantStatePlayer::Start()
{
	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Enchant, false);
}

void EnchantStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void EnchantStatePlayer::End()
{
}

void RollStatePlayer::Start()
{
	auto actor{ GetActor() };
	auto rigid_body{ actor->GetRigidBody() };

	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::Roll, false);

	rigid_body->SuddenStop(1.0f);

	// �X�e�B�b�N���͕����ɉ��
	auto [lx, ly] { Input::GetInstance().GetGamePad().GetStickL() };

	Vector3 dir{ };

	// �X�e�B�b�N���͂�����Ă��Ȃ��ꍇ
	if (lx < FLT_EPSILON && lx > -FLT_EPSILON && ly < FLT_EPSILON && ly > -FLT_EPSILON)
	{
		// ���݌����Ă�������Ƀ��[�����O
		dir = Matrix::CreateRotation(actor->GetWorldRotation()).m[2]; 
	}
	else
	{
		auto camera{ Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera() };

		const auto& forward{ camera->GetForward() };
		const auto& right{ camera->GetRight() };

		// ���͂����̎��� 1.0f ���̎��� -1.0f
		dir += right.Normalize() * ((lx > FLT_EPSILON ) ? 1.0f : (lx < -FLT_EPSILON) ? -1.0f : 0.0f);
		dir += forward.Normalize() * ((ly > FLT_EPSILON ) ? 1.0f : (ly < -FLT_EPSILON) ? -1.0f : 0.0f);
		dir.y = 0.0f;
		dir = dir.Normalize();
		actor->GetComponent<Character>()->SetFaceDirection(dir, 10.0f);
	}

	rigid_body->AddVelocity(dir * 6.0f);
	rigid_body->AddVelocity(Vector3(0.0f, 1.0f, 0.0f));
}

void RollStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	if (GameSys.GetPlayerController()->CanTransitionJumpAttack())
	{
		if (Input::GetInstance().GetGamePad().GetButton(GamePad::RIGHT_SHOULDER, GamePad::DOWN))
		{
			GetStateMachine()->SetNextSubState(ActorPlayer::_JumpAttackState);
			return;
		}
	}

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		// �W�����v�U�����s��Ȃ��i�o���Ȃ��j�̂Ń^�[�Q�b�g�̎Q�Ƃ�����
		GameSys.GetPlayerController()->ClearJumpAttackTarget();
		return;
	}
}

void RollStatePlayer::End()
{
}

void JumpAttackStatePlayer::Start()
{
	auto actor{ GetActor() };

	GetActor()->GetAnimator()->PlayAnimation(ActorPlayer::JumpAttack, false);

	auto target{ GameSys.GetPlayerController()->GetJumpAttackTarget() };

	Vector3 dir{ target->GetWorldPosition() - actor->GetWorldPosition()};

	// ���x�ł͂Ȃ����`��Ԃňړ�������(�y)
	actor->GetRigidBody()->SuddenStop(1.0f);
	actor->GetComponent<Character>()->SetFaceDirection(dir.Normalize(), 10.0f);
}

void JumpAttackStatePlayer::Execute(float elapsed_time)
{
	auto actor{ GetActor() };
	auto animator{ actor->GetAnimator() };

	// �U���Ώۂ����Ȃ��Ȃ����狭���I��
	auto target{ GameSys.GetPlayerController()->GetJumpAttackTarget() };

	if (!target)
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}

	// ���`��ԂŃ^�[�Q�b�g�̑O�̈ʒu�Ɉړ�����
	Vector3 vec{ actor->GetWorldPosition() - target->GetWorldPosition() };
	Vector3 goal{ target->GetWorldPosition() + vec.Normalize() * 0.5f };
	actor->SetWorldPosition(actor->GetWorldPosition().Lerp(goal, elapsed_time * 1.5f));


	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		GetStateMachine()->SetNextState(ActorPlayer::MovementState);
		GetStateMachine()->SetNextSubState(ActorPlayer::_IdleState);
		return;
	}
}

void JumpAttackStatePlayer::End()
{
	// �^�[�Q�b�g����
	GameSys.GetPlayerController()->ClearJumpAttackTarget();
}
