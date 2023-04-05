#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "WeaponEnchant.h"

/* --- 宣言 ---- */

class WeaponEnchantSpark: public WeaponEnchant
{
public:

	WeaponEnchantSpark()
		: WeaponEnchant() {};
	~WeaponEnchantSpark() = default;

	void PlaySlashTrail(const std::shared_ptr<Actor>& actor) override;

	void CollideSlash(const std::shared_ptr<Weapon>& weapon);

private:

	const char* GetWeaponEffectPath()        const override { return "Spark/spark_weapon.efk"; };

	void AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;

};

