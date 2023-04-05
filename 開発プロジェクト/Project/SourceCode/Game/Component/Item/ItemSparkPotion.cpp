#include "ItemSparkPotion.h"


/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../General/GameSystem.h"
#include "../Item/ItemDataBase.h"
#include "../Weapon/Weapon.h"
#include "../Weapon/Enchant/WeaponEnchantSpark.h"

/* --- 定義 ---- */

void ItemSparkPotion::Start()
{
	effect_handle.Initialize("Spark/spark_potion.efk", GetActor()->GetWorldPosition());
}

void ItemSparkPotion::Update(float elapsed_time)
{
	effect_handle.SetPosition(GetActor()->GetWorldPosition());
}

void ItemSparkPotion::End()
{
	effect_handle.Destroy();
}

bool ItemSparkPotion::CanEnchant(const std::shared_ptr<Actor>& opponent_actor)
{
	return HasWeapon(opponent_actor);
}

bool ItemSparkPotion::Enchant(const std::shared_ptr<Actor>& weapon_actor)  
{
	weapon_actor->GetComponent<Weapon>()->SetWeaponEnchant(std::make_unique<WeaponEnchantSpark>());

	return true;
}

const ItemDataBase::Data& ItemSparkPotion::GetItemData()
{
	return GameSystem::GetInstance().GetItemDataBase()->GetData(ItemDataBase::List::SparkPotion);
}
