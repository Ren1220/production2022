#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "WeaponEnchant.h"

/* --- 宣言 ---- */

class WeaponEnchantPoison : public WeaponEnchant
{
public:

	WeaponEnchantPoison()
		: WeaponEnchant() {};
	~WeaponEnchantPoison() = default;


private:

	const char* GetWeaponEffectPath()        const override { return "Poison/poison.efk"; };

	void AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;
	void AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) override;


};

