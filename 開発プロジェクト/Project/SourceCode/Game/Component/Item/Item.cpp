#include "Item.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../Actor/Actor.h"
#include "../Weapon/WeaponHandler.h"

/* --- 定義 ---- */

bool Item::HasWeapon(const std::shared_ptr<Actor>& dst) const
{
    return dst->GetComponent<WeaponHandler>()->GetWeaponActor() != nullptr;
}
