#include "ItemPoisonPotion.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../General/GameSystem.h"
#include "../Item/ItemDataBase.h"
#include "../Health/Health.h"
#include "../Weapon/Weapon.h"
#include "../Weapon/Enchant/WeaponEnchantPoison.h"

/* --- 追加のヘッダファイルのインクルード ---- */

ItemPoisonPotion::ItemPoisonPotion(int32_t recover_value)
	: Item(), damage_value(damage_value)
{
}

void ItemPoisonPotion::ImGuiRender()
{
}

bool ItemPoisonPotion::CanConsume(const std::shared_ptr<Actor>& opponent_actor)
{
	return true;
}

bool ItemPoisonPotion::CanEnchant(const std::shared_ptr<Actor>& opponent_actor)
{
	return HasWeapon(opponent_actor);
}

bool ItemPoisonPotion::Consume(const std::shared_ptr<Actor>& opponent_actor)
{
	if (auto health = opponent_actor->GetComponent<Health>())
	{
		health->Recover(damage_value);
		return true;
	}

	return false;
}

bool ItemPoisonPotion::Enchant(const std::shared_ptr<Actor>& weapon_actor)
{
	weapon_actor->GetComponent<Weapon>()->SetWeaponEnchant(std::make_unique<WeaponEnchantPoison>());

	return true;
}

const ItemDataBase::Data& ItemPoisonPotion::GetItemData()
{
	return GameSystem::GetInstance().GetItemDataBase()->GetData(ItemDataBase::List::PoisonPotion);
}


