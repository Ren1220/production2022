#include "ItemFirePotion.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../General/GameSystem.h"
#include "../Item/ItemDataBase.h"
#include "../Weapon/Weapon.h"
#include "../Weapon/Enchant/WeaponEnchantFire.h"

/* --- 定義 ---- */

void ItemFirePotion::Start()
{
	effect_handle.Initialize("Fire/fire.efk", GetActor()->GetWorldPosition());
}

void ItemFirePotion::Update(float elapsed_time)
{
	effect_handle.SetPosition(GetActor()->GetWorldPosition());
}

void ItemFirePotion::End()
{
	effect_handle.Destroy();
}

bool ItemFirePotion::CanConsume(const std::shared_ptr<Actor>& opponent_actor)
{
	return true;
}

bool ItemFirePotion::CanEnchant(const std::shared_ptr<Actor>& opponent_actor)
{
	return HasWeapon(opponent_actor);
}

bool ItemFirePotion::Enchant(const std::shared_ptr<Actor>& weapon_actor)
{
	weapon_actor->GetComponent<Weapon>()->SetWeaponEnchant(std::make_unique<WeaponEnchantFire>());

	return true;	
}

const ItemDataBase::Data& ItemFirePotion::GetItemData()
{
	return GameSystem::GetInstance().GetItemDataBase()->GetData(ItemDataBase::List::FirePotion);
}
