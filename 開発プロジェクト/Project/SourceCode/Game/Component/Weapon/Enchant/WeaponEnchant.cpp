#include "WeaponEnchant.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <string_view>

#include "Game/General/BattleSystem.h"
#include "Audio/AudioHandle.h"

#include "../../Health/Health.h"
#include "../Weapon.h"

/* --- 定義 ---- */

WeaponEnchant::~WeaponEnchant()
{
}

void WeaponEnchant::Initialize(const Vector3& position)
{
	std::string_view filepath{ GetWeaponEffectPath() };

	if (!filepath.empty())
	{
		weapon_effect = std::make_unique<EffekseerHandle>();
		weapon_effect->Initialize(filepath.data(), position);
	}
}

void WeaponEnchant::EndProcess()
{
	if (weapon_effect)
	{
		weapon_effect->Destroy();
	}
}

void WeaponEnchant::ShowWeaponEffect()
{
	if (weapon_effect)
	{
		weapon_effect->SetShown(true);
	}
}

void WeaponEnchant::HideWeaponEffect()
{
	if (weapon_effect)
	{
		weapon_effect->SetShown(false);
	}
}

void WeaponEnchant::AttackByPlayer(
	const std::shared_ptr<Actor>& player, 
	const std::shared_ptr<Actor>& target, 
	int32_t weapon_base_damage)
{
	ActorPlayer::State state{
		static_cast<ActorPlayer::State>(
			player->GetBehavior<StateMachine>()->GetCurrentSubStateKey().value_or(-1)) };

	switch (state)
	{
	case ActorPlayer::_AttackCombo0:
		AttackCombo0(player, target, weapon_base_damage);
		break;
	case ActorPlayer::_AttackCombo1:
		AttackCombo1(player, target, weapon_base_damage);
		break;
	case ActorPlayer::_AttackCombo2:
		AttackCombo2(player, target, weapon_base_damage);
		break;
	case ActorPlayer::_AttackSlash:
		AttackSlash(player, target, weapon_base_damage);
		break;
	case ActorPlayer::_JumpAttackState:
		AttackJumpAtk(player, target, weapon_base_damage);
		break;
	default:
		_ASSERT(0);
		break;
	}
}

void WeaponEnchant::CollideCombo0(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.0f, 0.25f);
}

void WeaponEnchant::CollideCombo1(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.0f, 0.25f);
}

void WeaponEnchant::CollideCombo2(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.0f, 0.25f);
}

void WeaponEnchant::CollideSlash(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.0f, 0.25f);
}

void WeaponEnchant::CollideJumpAtk(const std::shared_ptr<Weapon>& weapon)
{
	weapon->CollideCapsule(1.0f, 0.25f);
}

