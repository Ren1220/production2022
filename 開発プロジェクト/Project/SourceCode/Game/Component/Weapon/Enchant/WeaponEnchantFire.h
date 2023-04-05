#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "WeaponEnchant.h"

/* --- 宣言 ---- */

class WeaponEnchantFire : public WeaponEnchant
{
public:

	WeaponEnchantFire()
		: WeaponEnchant() {};
	~WeaponEnchantFire() = default;

	void PlaySlashTrail(const std::shared_ptr<Actor>& actor) override;

	void CollideSlash(const std::shared_ptr<Weapon>& weapon);
	void CollideJumpAtk(const std::shared_ptr<Weapon>& weapon);

private:

	const char* GetWeaponEffectPath() const override { return "Fire/fire.efk"; };
	
	void AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;

	EffekseerHandle jump_atk_efk;
};

