#include "BehaviorActionSpider.h"

/* --- 追加のヘッダファイルのインクルード ---- */

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

/* --- 定義 ---- */

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

	// 特に何もしない

	return State::Run;
}

void ActionDamagedSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	// TODO ダメージアニメーション作ってない
	actor->GetAnimator()->PlayAnimation(ActorSpider::AnimationIndex::Walk, false);
	actor->GetRigidBody()->SuddenStop(1.0f);
	actor->GetBehavior<BehaviorSpider>()->SetDamaged(false);
}

BehaviorAction::State ActionDamagedSpider::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Failed;
	}

	// 被ダメージアニメーション終了
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

	// 被ダメージ
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		actor->GetBehavior<BehaviorSpider>()->SetDamaged(true);
		return State::Failed;
	}

	// ターゲットを見失う
	const Vector3& player_vec{ behavior->GetPlayerVector() };
	float player_dist{ player_vec.Length() };

	if (player_dist > behavior->GetSearchRange())
	{
		return State::Failed;
	}

	// 攻撃遷移範囲にターゲットが存在する
	float attack_dist{ behavior->GetAttackDistance() };
	float attack_point_range{ behavior->GetAttackPointRange() };

	// プレイヤーとの距離が攻撃地点の範囲である
	if (fabsf(player_dist - attack_dist) < attack_point_range)
	{
		return State::Complete;
	}

	// 攻撃位置に移動する
	Vector3 move_point{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() - player_vec.Normalize() * attack_dist};
	Vector3 move_direction{ (move_point - actor->GetWorldPosition()).Normalize() };
	// 距離が近いときは離れる
//	if (player_dist < attack_dist)
//	{
//		move_direction *= -1;
//	}

	// 壁に衝突したらその地点から攻撃する
	if (actor->GetRigidBody()->IsCollideWall())
	{
		return State::Complete;
	}

	// 攻撃地点に移動
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
	// 被ダメージ
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

	// プレイヤーの方向を向いた
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

	// 被ダメージ
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

	// 被ダメージ
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

	// 被ダメージ
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	// 攻撃アニメーション終了
	if (animator->HasPlayedAnimation())
	{
		return State::Failed;
	}

	return State::Run;
}

void ActionWanderSpider::Begin(const std::shared_ptr<Actor>& actor)
{
	// 目標地点を設定
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

	// 被ダメージ
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	float player_dist_sq{ behavior->GetPlayerVector().LengthSquare() };

	// 探知範囲にプレイヤーが存在する
	if (player_dist_sq < behavior->GetSearchRange() * behavior->GetSearchRange())
	{
		return State::Complete;
	}

	// 目標地点に近い位置にいる
	if (actor->GetWorldPosition().NearEqual(behavior->GetWanderGoalPoint(), Vector3(0.5f, 0.5f, 0.5f)))
	{
		// 新しい目標地点を設定する
		SetWanderGoalPoint(actor);
	}

	// 壁に衝突
	if (rigid_body->IsCollideWall())
	{
		// 新しい目標地点を設定する
		SetWanderGoalPoint(actor);
	}

	// 徘徊
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

