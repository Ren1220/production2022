#include "BehaviorActionBoss.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorBoss.h"
#include "Game/Animation/Animator.h"
#include "Game/Component/Health/Health.h"
#include "Game/Component/Character.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/Physics/RigidBody.h"

#include "BehaviorBoss.h"

/* --- 定義 ---- */

void ActionDeadBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Dead, false);
}

BehaviorAction::State ActionDeadBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	return State::Run;
}

void ActionDamagedBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Damaged, false);
}

BehaviorAction::State ActionDamagedBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	return State::Run;
}

void ActionIdleBoss::Begin(const std::shared_ptr<Actor>& actor)
{
actor->GetAnimator()->PlayAnimation(ActorBoss::Idle, true);
}

BehaviorAction::State ActionIdleBoss::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorBoss>() };

	const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
	float dist{ actor->GetWorldPosition().Distance(player_pos) };

	// プレイヤーを発見
	if (dist < behavior->GetSearchRange())
	{
		// 発見フラグ
		behavior->SetFindPlayer(true);


		return State::Complete;
	}

	return State::Run;
}

void ActionRoaringBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::RoaringBegin, false);
	actor->GetBehavior<BehaviorBoss>()->SetModeChanged(false);
}

BehaviorAction::State ActionRoaringBoss::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionRoaringBattleBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Roaring, false);
	actor->GetBehavior<BehaviorBoss>()->SetModeChanged(false);
}

BehaviorAction::State ActionRoaringBattleBoss::Run(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionPursuitBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	// 現在の状態に応じてアニメーションが変わる
	switch (actor->GetBehavior<BehaviorBoss>()->GetMode())
	{
	case BehaviorBoss::Usual:

		// 歩き
		actor->GetAnimator()->PlayAnimation(ActorBoss::Walk, true);

		actor->GetComponent<Character>()->SetMaxMoveSpeed(1.0f);
		actor->GetComponent<Character>()->SetMoveSpeed(1.0f);

		break;
	case BehaviorBoss::Enraged:
		// 走り
		actor->GetAnimator()->PlayAnimation(ActorBoss::Run, true);

		actor->GetComponent<Character>()->SetMaxMoveSpeed(3.0f);
		actor->GetComponent<Character>()->SetMoveSpeed(1.2f);

		break;
	case BehaviorBoss::Desperate:
		// 走り
		actor->GetAnimator()->PlayAnimation(ActorBoss::Run, true);

		actor->GetComponent<Character>()->SetMaxMoveSpeed(4.0f);
		actor->GetComponent<Character>()->SetMoveSpeed(1.2f);
		actor->GetBehavior<BehaviorBoss>()->SetAttackRange(5.0f);

		break;
	}
}

BehaviorAction::State ActionPursuitBoss::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
	Vector3 vec{ player_pos - actor->GetWorldPosition() };

	// プレイヤーの方向に移動
	actor->GetComponent<Character>()->Move(vec.Normalize(), 10.0f);

	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	// 攻撃範囲にプレイヤー
	if (actor->GetBehavior<BehaviorBoss>()->GetAttackRange() > vec.Length())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionBackBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
	if ((player_pos - actor->GetWorldPosition()).Length() < actor->GetBehavior<BehaviorBoss>()->GetTransitionBackRange())
	{
		actor->GetAnimator()->PlayAnimation(ActorBoss::BackJump, false);
	}
}

BehaviorAction::State ActionBackBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{

	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	// プレイヤーからすでに離れている場合はアニメーションを再生せずそのままCompleteを返す
	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionFaceBoss::Begin(const std::shared_ptr<Actor>& actor)
{
}

BehaviorAction::State ActionFaceBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };
	Vector3 vec{ player_pos - actor->GetWorldPosition() };
	
	Matrix rot{ Matrix::CreateRotation(actor->GetWorldRotation()) };
	float d{ vec.Normalize().Dot(Vector3(rot.m[2])) };
	if (d > 0.90f)
	{
		return State::Complete;
	}

	// プレイヤーの方向を向く
	actor->GetComponent<Character>()->SetFaceDirection(vec.Normalize(), 10.0f);

	return State::Run;
}

void ActionAttack0Boss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Attack0, false);
}

BehaviorAction::State ActionAttack0Boss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionAttack1Boss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Attack1, false);
}

BehaviorAction::State ActionAttack1Boss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionTackleBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Tackle, false);
}

BehaviorAction::State ActionTackleBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionLaunchBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Kick, false);
}

BehaviorAction::State ActionLaunchBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionJumpATKBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::JumpAttack, false);
}

BehaviorAction::State ActionJumpATKBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}

void ActionReturnBoss::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorBoss::Return, false);
}

BehaviorAction::State ActionReturnBoss::Run(
	float elapsed_time, 
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	if (actor->GetAnimator()->HasPlayedAnimation())
	{
		return State::Complete;
	}

	return State::Run;
}
