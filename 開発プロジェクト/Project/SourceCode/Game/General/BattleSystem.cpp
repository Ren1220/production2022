#include "BattleSystem.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Game/Actor/Actor.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Component/Health/Health.h"
#include "Game/General/GameSystem.h"

/* --- ��` ---- */

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

		// �_���[�W�������s��
		bool result = tgt->GetComponent<Health>()->Damaged(atk, combat.damage, combat.invinsible_time, combat.camera_shake_time);

		// �ǉ��Ŏ��s����֐������݂���ꍇ���s����
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
