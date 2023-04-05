#include "BehaviorActionSpider.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/RandomFunction.h"

#include "Game/Actor/ActorSpider.h"
#include "Game/Animation/Animator.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/Collider/ColliderManager.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/Physics/RigidBody.h"

#include "BehaviorSpider.h"

/* --- ��` ---- */

void ActionDeadSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSpider::Dead, false);
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionDeadSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto rigid_body{ actor->GetRigidBody() };

	// ���ɉ������Ȃ�

	return State::Run;
}

void ActionDamagedSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	// TODO �_���[�W�A�j���[�V��������ĂȂ�
	actor->GetAnimator()->PlayAnimation(ActorSpider::AnimationIndex::Walk, false);
	actor->GetRigidBody()->SuddenStop(1.0f);
	actor->GetBehavior<BehaviorSpider>()->SetDamaged(false);
}

BehaviorAction::State ActionDamagedSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	// �r���ő̗͂��[���ɂȂ����ꍇ�A�j���[�V�����������I�����A���S����B
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	// ��_���[�W�A�j���[�V�����I��
	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionPursuitSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSpider::Walk, true);
}

BehaviorAction::State ActionPursuitSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSpider>() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		actor->GetBehavior<BehaviorSpider>()->SetDamaged(true);
		return State::Failed;
	}

	// �^�[�Q�b�g��������
	const Vector3& player_vec{ behavior->GetPlayerVector() };
	float player_dist{ player_vec.Length() };

	if (player_dist > behavior->GetSearchRange())
	{
		return State::Failed;
	}

	// �U���J�ڔ͈͂Ƀ^�[�Q�b�g�����݂���
	float attack_dist{ behavior->GetAttackDistance() };
	float attack_point_range{ behavior->GetAttackPointRange() };

	// �v���C���[�Ƃ̋������U���n�_�͈̔͂ł���
	if (fabsf(player_dist - attack_dist) < attack_point_range)
	{
		return State::Complete;
	}

	// �U���ʒu�Ɉړ�����
	Vector3 move_point{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() - player_vec.Normalize() * attack_dist};
	Vector3 move_direction{ (move_point - actor->GetWorldPosition()).Normalize() };
	// �������߂��Ƃ��͗����
//	if (player_dist < attack_dist)
//	{
//		move_direction *= -1;
//	}

	// �ǂɏՓ˂����炻�̒n�_����U������
	if (actor->GetRigidBody()->IsCollideWall())
	{
		return State::Complete;
	}

	// �U���n�_�Ɉړ�
	actor->GetComponent<Character>()->SetMoveSpeed(behavior->GetPursuitSpeed());
	actor->GetComponent<Character>()->SetMaxMoveSpeed(behavior->GetPursuitSpeedMax());
	actor->GetComponent<Character>()->Move(move_direction, 10.0f);

	return State::Run;
}

void ActionFaceSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetRigidBody()->SuddenStop(1.0f);
}

BehaviorAction::State ActionFaceSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		actor->GetBehavior<BehaviorSpider>()->SetDamaged(true);
		return State::Failed;
	}

	Vector3 face_direction{ (GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() - actor->GetWorldPosition()).Normalize() };

	actor->GetComponent<Character>()->SetFaceDirection(face_direction, 10.0f);

	Vector3 forward_vector{ Matrix::CreateRotation(actor->GetWorldRotation()).m[2] };

	//float distance_to_player{ actor->GetBehavior<BehaviorEnemy>()->CalcDistanceToPlayer() };
	//if (distance_to_player > actor->GetBehavior<BehaviorEnemy>()->GetAttackRange())
	//{
	//	return State::Failed;
	//}

	// �v���C���[�̕�����������
	float dot{ forward_vector.Dot(face_direction) };
	if (dot > 0.98f)
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionApproachSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSpider::Jump, false);
}

BehaviorAction::State ActionApproachSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		actor->GetBehavior<BehaviorSpider>()->SetDamaged(true);
		return State::Failed;
	}

	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionAttackSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSpider::Bite, false);
}

BehaviorAction::State ActionAttackSpider::Run(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		actor->GetBehavior<BehaviorSpider>()->SetDamaged(true);
		return State::Failed;
	}

	if (animator->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionLaunchSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorSpider::Launch, false);
}

BehaviorAction::State ActionLaunchSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto behavior{ actor->GetBehavior<BehaviorSpider>() };

	// ��_���[�W
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	// �U���A�j���[�V�����I��
	if (animator->HasPlayedAnimation())
	{
		return State::Failed;
	}

	return State::Run;
}

void ActionWanderSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	// �ڕW�n�_��ݒ�
	SetWanderGoalPoint(actor);

	actor->GetAnimator()->PlayAnimation(ActorSpider::Walk, true);
}

BehaviorAction::State ActionWanderSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };
	auto rigid_body{ actor->GetRigidBody() };
	auto behavior{ actor->GetBehavior<BehaviorSpider>() };

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

void ActionWanderSpider::SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const
{
	auto behavior{ actor->GetBehavior<BehaviorSpider>() };

	float wander_range{ behavior->GetWanderRange() };
	Vector3 wander_goal_point{ actor->GetWorldPosition() };

	wander_goal_point.x += Random::GetValue(-wander_range, wander_range);
	wander_goal_point.z += Random::GetValue(-wander_range, wander_range);

	behavior->SetWanderGoalPoint(wander_goal_point);
}

