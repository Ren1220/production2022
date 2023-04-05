#include "PlayerController.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorManager.h"
#include "Game/HUD/HUDManager.h"
#include "Game/Component/Health/Health.h"
#include "Input/Input.h"

/* --- 定義 ---- */

PlayerController::PlayerController()
	: w_player(), item_pickup_range(), can_transition_jump_attack(false)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Initialize()
{
	item_pickup_range = 2.0f;
}

void PlayerController::Update(float elapsed_time)
{
	//TargetEnemy(); 未実装
}

void PlayerController::EndProcess()
{
}

void PlayerController::DeadPlayer()
{
	if (auto player = w_player.lock())
	{
		// 死亡判定
		if (player->GetComponent<Health>()->IsDead())
		{
			GameSys.GetHUDManager()->SetShowHUDFlag(false);

		}
	}
}

void PlayerController::ImGuiRender()
{
}

void PlayerController::SetPlayer(const std::shared_ptr<Actor>& player)
{
	w_player = player;
}

std::shared_ptr<Actor> PlayerController::GetPlayer() const
{
	return w_player.lock();
}

std::shared_ptr<Actor> PlayerController::GetTarget() const
{
	return w_target.lock();
}

std::shared_ptr<Actor> PlayerController::GetJumpAttackTarget() const
{
	return w_jump_attack_target.lock();
}

void PlayerController::SelectJumpAttackTarget(const std::vector<std::weak_ptr<Actor>>& w_attackers)
{
	auto actor{ w_player.lock() };

	float min_dist_sq{ FLT_MAX };

	// 攻撃してきた相手の中で最近の敵を攻撃対象にする
	for (const auto& wa : w_attackers)
	{
		if (wa.expired()) continue;

		auto attacker{ wa.lock() };

		float dist_sq{ (attacker->GetWorldPosition() - actor->GetWorldPosition()).LengthSquare() };

		if (dist_sq < min_dist_sq)
		{
			min_dist_sq = dist_sq;
			w_jump_attack_target = attacker;
		}
	}
}

void PlayerController::ClearJumpAttackTarget()
{
	w_jump_attack_target.reset();
}

void PlayerController::TargetEnemy()
{
	const auto& input{ Input::GetInstance() };
	const auto& game_pad{ input.GetGamePad() };

	if (game_pad.GetButton(GamePad::RIGHT_THUMB, GamePad::Flag::DOWN))
	{
		// ターゲットしている対象が存在しない場合ターゲットを行う
		if (w_target.expired())
		{
			w_target = SearchTarget();
		}
		// 既にターゲットしている場合ターゲットを解除
		else
		{
			w_target.reset();
		}
	}
}

std::shared_ptr<Actor> PlayerController::SearchTarget()
{
	for (const auto& actor : GameSys.GetActorManager()->GetUpdateActors())
	{
		if (!actor->CompareGroupName("Enemy")) continue;

		// TODO
		return actor;
	}

	return nullptr;
}
