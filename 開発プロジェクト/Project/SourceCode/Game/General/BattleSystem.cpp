#include "BattleSystem.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Component/Health/Health.h"
#include "Game/General/GameSystem.h"

/* --- 定義 ---- */

void BattleSystem::Initialize()
{
}

void BattleSystem::ResetHealthFlags()
{
	for (const auto& a : GameSys.GetActorManager()->GetUpdateActors())
	{
		if (auto health = a->GetComponent<Health>())
		{
			health->ResetDamagedFlag();
		}
	}
}

void BattleSystem::CombatProcess()
{
	for (const auto& combat : combat_datas)
	{
		auto atk{ combat.w_attacker.lock() };
		auto tgt{ combat.w_target.lock() };

		_ASSERT(atk && tgt);

		// ダメージ処理を行う
		bool result = tgt->GetComponent<Health>()->Damaged(atk, combat.damage, combat.invinsible_time, combat.camera_shake_time);

		// 追加で実行する関数が存在する場合実行する
		if (combat.combat_callback)
		{
			combat.combat_callback(atk.get(), tgt.get(), result);
		}
	}

	combat_datas.clear();
}

void BattleSystem::SendCombatData(
	const std::shared_ptr<Actor>& attacker, 
	const std::shared_ptr<Actor>& target, 
	int32_t damage,
	float invinsible_time,
	float camera_shake_time,
	const std::function<void(Actor*, Actor*, bool)>& combat_callback)
{
	combat_datas.emplace_back(attacker, target, damage, invinsible_time, camera_shake_time, combat_callback);
}
