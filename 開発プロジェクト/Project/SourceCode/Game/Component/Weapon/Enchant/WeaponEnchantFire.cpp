#include "WeaponEnchantFire.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/General/BattleSystem.h"
#include "Graphics/General/Graphics.h"
#include "Audio/AudioHandle.h"
#include "../Weapon.h"

/* --- 定義 ---- */

void WeaponEnchantFire::PlaySlashTrail(const std::shared_ptr<Actor>& actor)
{
	EffekseerHandle trail_efk;
	trail_efk.Initialize("Fire/fire_slash.efk", actor->GetWorldPosition() + Vector3(0, 1, 0));
	trail_efk.SetScale({ 0.5f,0.5f,0.5f });
	trail_efk.SetRotation(actor->GetWorldRotation());
}

void WeaponEnchantFire::CollideSlash(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(2.0f, 0.25f);
}

void WeaponEnchantFire::CollideJumpAtk(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideSphere(2.5f);
}

void WeaponEnchantFire::AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 15, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Fire/fire_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantFire::AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 15, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Fire/fire_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantFire::AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 30, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Fire/fire_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantFire::AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 30, 0.0f, 0.2f,
		[](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				EffekseerHandle efk;
				efk.Initialize("Fire/fire_hit.efk", target->GetWorldPosition());

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

void WeaponEnchantFire::AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage)
{
	BattleSystem::GetInstance().SendCombatData(player, target, 50, 0.0f, 0.2f,
		[&](Actor* attacker, Actor* target, bool is_damaged)
		{
			if (is_damaged)
			{
				//EffekseerHandle efk;
				if (!jump_atk_efk.IsExist())
				{
					jump_atk_efk.Initialize("Fire/fire_jump_attack_hit.efk", target->GetWorldPosition());
				}

				AudioHandle hit;
				hit.Play(L"Data/Audio/SE/効果音ラボ/剣で斬る2.wav");
			}
		});
}

//void WeaponEnchantFire::SelectHitEffectByPlayer(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target)
//{
//	ActorPlayer::State state{ player->GetBehavior<StateMachine>()->GetCurrentSubStateKey().value_or(0U) };
//
//	if (state == ActorPlayer::_JumpAttackState)
//	{
//		// エフェクトのインスタンスが存在していない場合のみエフェクトをプレイヤーの位置に再生
//		if (effects[Effect::E_JumpAttackHit])
//		{
//			if (!effects[Effect::E_JumpAttackHit]->IsExist())
//			{
//				PlayEffect(E_JumpAttackHit, player->GetWorldPosition());
//			}
//		}
//		else
//		{
//			PlayEffect(E_JumpAttackHit, player->GetWorldPosition());
//		}
//	}
//	else if (state == ActorPlayer::_AttackSlash)
//	{
//		PlayEffect(E_SlashHit, target->GetWorldPosition());
//	}
//	else
//	{
//		PlayEffect(E_Hit, target->GetWorldPosition());
//	}
//}

