#include "BehaviorActionWolf.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/RandomFunction.h"
#include "General/AtaiEngine.h"


#include "Game/Actor/ActorWolf.h"
#include "Game/Animation/Animator.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/Physics/RigidBody.h"
#include "Game/Stage/StageManager.h"

#include "BehaviorWolf.h"

/* --- ��` ---- */

void ActionPursuitWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Run, true);
}

BehaviorAction::State ActionPursuitWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	float player_dist_sq{ behavior->GetPlayerVector().LengthSquare() };

	// �^�[�Q�b�g��������
	if (player_dist_sq > behavior->GetPursuitRange() * behavior->GetPursuitRange())
	{
		return State::Failed;
	}

	// �v���C���[�Ƃ̋������v������
	const Vector3& pos{ actor->GetWorldPosition() };
	const Vector3& player_pos{ GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() };

	float distance_to_player = pos.Distance(player_pos);

	// �U���J�ڔ͈͂Ƀ^�[�Q�b�g�����݂���
	if (player_dist_sq < behavior->GetAttackRange() * behavior->GetAttackRange())
	{
		return State::Complete;
	}

	// �ǐ�
	Vector3 move_direction{ behavior->GetPlayerVector().Normalize() };

	actor->GetComponent<Character>()->SetMoveSpeed(behavior->GetPursuitSpeed());
	actor->GetComponent<Character>()->SetMaxMoveSpeed(behavior->GetPursuitSpeedMax());
	actor->GetComponent<Character>()->Move(move_direction, 10.0f);

	return State::Run;
}

void ActionAttackWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Attack, false);
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	behavior->SetCoolTime(behavior->GetAttackCoolTime());
}

BehaviorAction::State ActionAttackWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged(true);
		return State::Failed;
	}

	if (animator->HasPlayedAnimation() && behavior->GetCoolTimer() < 0.0f)
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionWanderWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	// �ڕW�n�_��ݒ�
	SetWanderGoalPoint(actor);

	actor->GetAnimator()->PlayAnimation(ActorWolf::Walk, true);
}

BehaviorAction::State ActionWanderWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto rigid_body{ actor->GetRigidBody() };
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };


	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	float player_dist_sq{ behavior->GetPlayerVector().LengthSquare() };

	// �T�m�͈͂Ƀv���C���[�����݂���
	if (player_dist_sq < behavior->GetSearchRange() * behavior->GetSearchRange())
	{
		return State::Complete;
	}

	// �ڕW�n�_�ɋ߂��ʒu�ɂ���
	if (actor->GetWorldPosition().NearEqual(behavior->GetWanderGoalPoint(), Vector3(0.5f, 0.5f, 0.5f)))
	{
		// �V�����ڕW�n�_��ݒ肷��
		SetWanderGoalPoint(actor);
	}

	// �ǂɏՓ�
	if (rigid_body->IsCollideWall())
	{
		// �V�����ڕW�n�_��ݒ肷��
		SetWanderGoalPoint(actor);
	}

	// �p�j
	Vector3 move_direction{ (behavior->GetWanderGoalPoint() - actor->GetWorldPosition()).Normalize() };

	actor->GetComponent<Character>()->SetMoveSpeed(behavior->GetWanderSpeed());
	actor->GetComponent<Character>()->SetMaxMoveSpeed(behavior->GetWanderSpeedMax());
	actor->GetComponent<Character>()->Move(move_direction, 10.0f);

	return State::Run;
}

void ActionWanderWolf::SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	float wander_range{ behavior->GetWanderRange() };
	Vector3 wander_goal_point{ actor->GetWorldPosition() };

	wander_goal_point.x += Random::GetValue(-wander_range, wander_range);
	wander_goal_point.z += Random::GetValue(-wander_range, wander_range);

	behavior->SetWanderGoalPoint(wander_goal_point);
}

void ActionDamagedWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Damaged, false);
	actor->GetBehavior<BehaviorWolf>()->SetDamaged(false);
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionDamagedWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	auto rigid_body{ actor->GetRigidBody() };


	// �r���ő̗͂��[���ɂȂ����ꍇ�A�j���[�V�����������I�����A���S����B
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Complete;
	}

	// ��_���[�W�A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionDeadWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Dead, false);
}

BehaviorAction::State ActionDeadWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{

	// ���ɉ������Ȃ�

	return State::Run;
}

void ActionFaceWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionFaceWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	Vector3 face_direction{ (GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() - actor->GetWorldPosition()).Normalize() };

	actor->GetComponent<Character>()->SetFaceDirection(face_direction, 10.0f);

	Vector3 forward_vector{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

	DebugGUI::GetInstance().SetString(std::to_string(forward_vector.Dot(face_direction)));

	float player_dist_sq{ behavior->GetPlayerVector().LengthSquare() };
	// �v���C���[���U���͈͂��痣�ꂽ
	if (player_dist_sq > behavior->GetAttackRange() * behavior->GetAttackRange())
	{
		return State::Failed;
	}

	float dot{ forward_vector.Dot(face_direction) };
	if (dot > 0.9f)
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionSummonWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Howling, false);

	// ����
	int summon_count{ Random::GetValue(1, 2) };
	for (int i = 0; i < summon_count; ++i)
	{
		Vector3 pos{ actor->GetWorldPosition() };
		pos.x += Random::GetValue(-3.0f, 3.0f);
		pos.z += Random::GetValue(-3.0f, 3.0f);

		auto summoned{ GameSys.GetActorManager()->Create<ActorWolf>() };
		summoned->Initialize(MySystem.GetDevice(), std::format("{}_Kin{}", actor->GetName().c_str(), i).c_str(), pos, Quaternion::Identity(), { 1.25f, 1.25f, 1.25f }, true);
		// ���������G���X�e�[�W�̃A�N�^�Ƃ��ēo�^����B
		GameSys.GetStageManager()->RegisterActor(summoned);
	}
}

BehaviorAction::State ActionSummonWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	// ��_���[�W�A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}


	return State::Run;
}

void ActionSummonWolf::SummonWolf()
{

}

void ActionAppearanceWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Appearance, false);

	actor->GetComponent<Health>()->SetInvincibleTimer(1.0f);
}

BehaviorAction::State ActionAppearanceWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	// ��_���[�W�A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}


	return State::Run;
}

void ActionFaceSummonWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionFaceSummonWolf::Run(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	Vector3 face_direction{ (GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() - actor->GetWorldPosition()).Normalize() };

	actor->GetComponent<Character>()->SetFaceDirection(face_direction, 10.0f);

	Vector3 forward_vector{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

	float dot{ forward_vector.Dot(face_direction) };
	if (dot > 0.9f)
	{
		return State::Complete;
	}

	return State::Run;
}
