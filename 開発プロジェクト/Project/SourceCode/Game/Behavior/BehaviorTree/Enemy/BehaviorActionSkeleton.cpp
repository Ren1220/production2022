#include "BehaviorActionSkeleton.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/RandomFunction.h"

#include "Game/Actor/ActorSkeleton.h"
#include "Game/Animation/Animator.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/Physics/RigidBody.h"

#include "BehaviorSkeleton.h"

/* --- ��` ---- */

void ActionPursuitSkeleton::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSkeleton::RunCrawl, true, 10.0f);
}

BehaviorAction::State ActionPursuitSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

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

	// �U���J�ڔ͈͂Ƀ^�[�Q�b�g�����݂���
	if (player_dist_sq < behavior->GetAttackRange() * behavior->GetAttackRange())
	{
		return State::Complete;
	}

	// �ǐ�
	Vector3 move_direction{ behavior->GetPlayerVector().Normalize()};

	actor->GetComponent<Character>()->SetMoveSpeed(behavior->GetPursuitSpeed());
	actor->GetComponent<Character>()->SetMaxMoveSpeed(behavior->GetPursuitSpeedMax());
	actor->GetComponent<Character>()->Move(move_direction, 10.0f);

	return State::Run;
}

void ActionAttackSkeleton::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSkeleton::Attack, false);

	// ���̏�Ŏ~�܂�
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionAttackSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	// �A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionWanderSkeleton::Begin(const std::shared_ptr<Actor>& actor)
{
	// �ڕW�n�_��ݒ�
	SetWanderGoalPoint(actor);

	actor->GetAnimator()->PlayAnimation(ActorSkeleton::Walk, true, 10.0f);
}

BehaviorAction::State ActionWanderSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto rigid_body{ actor->GetRigidBody() };
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };


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

void ActionWanderSkeleton::SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const
{
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

	float wander_range { behavior->GetWanderRange() };
	Vector3 wander_goal_point{ actor->GetWorldPosition() };

	wander_goal_point.x += Random::GetValue(-wander_range, wander_range);
	wander_goal_point.z += Random::GetValue(-wander_range, wander_range);

	behavior->SetWanderGoalPoint(wander_goal_point);
}

void ActionDamagedSkeleton::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSkeleton::Scream, false, 10.0f);
	actor->GetBehavior<BehaviorSkeleton>()->SetDamaged(false);
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionDamagedSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };
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

void ActionDeadSkeleton::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSkeleton::Dead, false);
}

BehaviorAction::State ActionDeadSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	return State::Run;
}

BehaviorAction::State ActionFaceSkeleton::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

	Vector3 face_direction{ (GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() - actor->GetWorldPosition()).Normalize()};

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

