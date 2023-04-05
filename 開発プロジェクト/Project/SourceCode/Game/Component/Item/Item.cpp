#include "Item.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../../Actor/Actor.h"
#include "../Weapon/WeaponHandler.h"

/* --- ��` ---- */

bool Item::HasWeapon(const std::shared_ptr<Actor>& dst) const
{
    return dst->GetComponent<WeaponHandler>()->GetWeaponActor() != nullptr;
}
