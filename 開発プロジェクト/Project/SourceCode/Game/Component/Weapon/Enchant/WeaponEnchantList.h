#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <array>

#include "WeaponEnchant.h"
#include "WeaponEnchantFire.h"
#include "WeaponEnchantSpark.h"
#include "WeaponEnchantPoison.h"

/* --- 宣言 ---- */

class WeaponEnchantList
{
public:

	enum List
	{
		Fire,
		Spark,
		Poison,
		ElementNum,
	};

	static std::array<const char*, List::ElementNum> GetNames()
	{
		return
		{
			typeid(WeaponEnchantFire).name(),
			typeid(WeaponEnchantSpark).name(),
			typeid(WeaponEnchantPoison).name(),
		};
	}

	static void SetEnchantFromList(List id, std::unique_ptr<WeaponEnchant>& out_enchant)
	{
		switch (id)
		{
		case WeaponEnchantList::Fire:
			out_enchant = std::make_unique<WeaponEnchantFire>();
			break;
		case WeaponEnchantList::Spark:
			out_enchant = std::make_unique<WeaponEnchantSpark>();
			break;
		case WeaponEnchantList::Poison:
			out_enchant = std::make_unique<WeaponEnchantPoison>();
			break;
		default:
			break;
		}
	}

};
