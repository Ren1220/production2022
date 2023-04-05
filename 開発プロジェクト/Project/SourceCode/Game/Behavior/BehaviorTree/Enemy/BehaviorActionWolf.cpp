#include "BehaviorActionWolf.h"

/* --- 追加のヘッダファイルのインクルード ---- */

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

/* --- 定義 ---- */

void ActionPursuitWolf::Begin(const std::shared_ptr<Actor>& actor)
{
	actor->GetAnimator()->PlayAnimation(ActorWolf::Run, true);
}

BehaviorAction::State ActionPursuitWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	// 被ダメージ
	if (actor->GetComponent<Health>()->IsDamaged())
	{
		behavior->SetDamaged();
		return State::Failed;
	}

	float player_dist_sq{ behavior->GetPlayerVector().LengthSquare() };

	// ターゲットを見失う
	if (player_dist_sq > behavior->GetPursuitRange() * behavior->GetPursuitRange())
	{
		return State::Failed;
	}

	// プレイヤーとの距離を計測する
	const Vector3& pos{ actor->GetWorldPosition() };
	const Vector3& player_pos{ GameSystem::GetInstance().GetPlayerController()->GetPlayer()->GetWorldPosition() };

	float distance_to_player = pos.Distance(player_pos);

	// 攻撃遷移範囲にターゲットが存在する
	if (player_dist_sq < behavior->GetAttackRange() * behavior->GetAttackRange())
	{
		return State::Complete;
	}

	// 追跡
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

	// 被ダメージ
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
	// 目標地点を設定
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


	// 途中で体力がゼロになった場合アニメーションを強制終了し、死亡する。
	if (actor->GetComponent<Health>()->IsDead())
	{
		return State::Complete;
	}

	// 被ダメージアニメーション終了
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

	// 特に何もしない

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
	// プレイヤーが攻撃範囲から離れた
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

	// 召喚
	int summon_count{ Random::GetValue(1, 2) };
	for (int i = 0; i < summon_count; ++i)
	{
		Vector3 pos{ actor->GetWorldPosition() };
		pos.x += Random::GetValue(-3.0f, 3.0f);
		pos.z += Random::GetValue(-3.0f, 3.0f);

		auto summoned{ GameSys.GetActorManager()->Create<ActorWolf>() };
		summoned->Initialize(MySystem.GetDevice(), std::format("{}_Kin{}", actor->GetName().c_str(), i).c_str(), pos, Quaternion::Identity(), { 1.25f, 1.25f, 1.25f }, true);
		// 召喚した敵をステージのアクタとして登録する。
		GameSys.GetStageManager()->RegisterActor(summoned);
	}
}

BehaviorAction::State ActionSummonWolf::Run(
	float elapsed_time,
	const std::shared_ptr<Actor>& actor)
{
	auto animator{ actor->GetAnimator() };

	// 被ダメージアニメーション終了
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

	// 被ダメージアニメーション終了
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
