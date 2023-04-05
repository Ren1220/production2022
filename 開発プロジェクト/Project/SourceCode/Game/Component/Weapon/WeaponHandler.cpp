#include "WeaponHandler.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "../../Actor/Actor.h"

#include "Weapon.h"

/* --- 定義 ---- */

void WeaponHandler::Start()
{
	// 装備するアクタの武器をもつノードのアドレス
	w_weapon_joint_node = GetActor()->GetModel<GLTFSkinnedMesh>()->FindAnimatedNode("mixamorig:Sword_joint");
}

void WeaponHandler::SetWeaponActor(const std::shared_ptr<Actor>& weapon_actor)
{
	w_weapon_actor = weapon_actor;
}

std::shared_ptr<Actor> WeaponHandler::GetWeaponActor() const
{
	return w_weapon_actor.lock();
}

std::shared_ptr<GLTFModelData::Node> WeaponHandler::GetWeaponJointNode()
{
	return w_weapon_joint_node.lock();
}

void WeaponHandler::ChangeWeapon(const std::shared_ptr<Actor>& new_equip_weapon_actor)
{
	// 現在装備している武器が存在する場合
	if (auto current_equip_weapon_actor = w_weapon_actor.lock())
	{
		// 装備している武器をフィールドアイテム化
		current_equip_weapon_actor->GetComponent<Weapon>()->Unequip();
		w_weapon_actor.reset();
	}

	// 新しく装備する武器
	if (new_equip_weapon_actor != nullptr)
	{
		new_equip_weapon_actor->GetComponent<Weapon>()->Equip(GetActor());
		w_weapon_actor = new_equip_weapon_actor;
	}
}

