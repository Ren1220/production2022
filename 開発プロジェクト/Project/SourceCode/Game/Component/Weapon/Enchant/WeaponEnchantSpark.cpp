#include "WeaponEnchantSpark.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Audio/AudioHandle.h"
#include "Graphics/General/Graphics.h"

#include "Game/General/BattleSystem.h"
#include "../Weapon.h"

/* --- 定義 ---- */

//void WeaponEnchantSpark::Attack(
//	const std::shared_ptr<Component>& weapon,
//	const std::shared_ptr<Actor>& target, 
//	int32_t weapon_base_damage)
//{
//	DamageTo(target, weapon_base_damage * 30);
//	ReduceDurability(weapon, 2);
//
//	Vector3 pos{ target->GetWorldPosition() + Vector3(0.0f, 1.0f, 0.0f) };
//	Graphics::GetInstance().GetEffekseerManager().PlayEffect("Spark/spark_hit.efk", pos);
//}

//void WeaponEnchantSpark::AttackByPlayer(
//	const std::shared_ptr<Actor>& player, 
//	const std::shared_ptr<Actor>& target, 
//	int32_t weapon_base_damage)
//{
//	if (!player->GetBehavior<StateMachine>()->GetCurrentSubStateKey().has_value()) return;
//
//	ActorPlayer::State state{ player->GetBehavior<StateMachine>()->GetCurrentSubStateKey().value()};
//
//
//	int32_t damage_val = 0;
//
//	switch (state)
//	{
//	case ActorPlayer::_AttackCombo0:
//		damage_val = 15;
//		break;
//	case ActorPlayer::_AttackCombo1:
//		damage_val = 15;
//		break;
//	case ActorPlayer::_AttackCombo2:
//		damage_val = 30;
//		break;
//	case ActorPlayer::_AttackSlash:
//		damage_val = 50;
//		break;
//	case ActorPlayer::_JumpAttackState:
//
//		AudioHandle spark;
//		spark.Play(L"Data/Audio/SE/pixabay/hit-sound-effect-12445.wav", 0.5f);
//		//spark.ReverbEffect();
//
//		damage_val = 100;
//		break;
//	}
//
//	BattleSystem::GetInstance().SendCombatData(player, target, damage_val, 0.1f, 0.2f,
//		[](Actor* attacker, Actor* target, bool is_damaged)
//		{
//			if (is_damaged)
//			{
//				ActorPlayer::State state{ player->GetBehavior<StateMachine>()->GetCurrentSubStateKey().value_or(-1) };
//
//				if (state == ActorPlayer::_JumpAttackState)
//				{
//					PlayEffect(E_JumpAttackHit, target->GetWorldPosition());
//				}
//				else if (state == ActorPlayer::_AttackSlash)
//				{
//					PlayEffect(E_SlashHit, target->GetWorldPosition());
//				}
//				else
//				{
//					PlayEffect(E_Hit, target->GetWorldPosition());
//				}
//			}
//		});
//
//	if (DamageTo(target, damage_val))
//	{
//		SelectHitEffectByPlayer(player, target);
//	}
//}

void WeaponEnchantSpark::PlaySlashTrail(const std::shared_ptr<Actor>& actor)
{
	EffekseerHandle trail_efk;
	trail_efk.Initialize("Spark/spark_slash.efk", actor->GetWorldPosition() + Vector3(0, 1, 0));
	trail_efk.SetScale({ 0.5f,0.5f,0.5f });
	trail_efk.SetRotation(actor->GetWorldRotation());

	//if (efk)
	//{
	//	efk->SetScale({ 0.5f, 0.5f, 0.5f });
	//	efk->SetRotation(actor->GetWorldRotation());
	//}

}

void WeaponEnchantSpark::CollideSlash(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.5f, 0.25f);
}

void WeaponEnchantSpark::AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 15, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Spark/spark_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantSpark::AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 15, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Spark/spark_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantSpark::AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 30, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Spark/spark_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantSpark::AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 30, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Spark/spark_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantSpark::AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 100, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Spark/spark_jump_attack_hit.efk", target->GetWorldPosition());

				AudioHandle spark;
				spark.Play(L"Data/Audio/SE/pixabay/hit-sound-effect-12445.wav", 0.5f);
			}
		});
}
