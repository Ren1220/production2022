#include "WeaponHandler.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/Model/GLTFSkinnedMesh.h"

#include "../../Actor/Actor.h"

#include "Weapon.h"

/* --- ��` ---- */

void WeaponHandler::Start()
{
	// ��������A�N�^�̕�������m�[�h�̃A�h���X
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
	// ���ݑ������Ă��镐�킪���݂���ꍇ
	if (auto current_equip_weapon_actor = w_weapon_actor.lock())
	{
		// �������Ă��镐����t�B�[���h�A�C�e����
		current_equip_weapon_actor->GetComponent<Weapon>()->Unequip();
		w_weapon_actor.reset();
	}

	// �V�����������镐��
	if (new_equip_weapon_actor != nullptr)
	{
		new_equip_weapon_actor->GetComponent<Weapon>()->Equip(GetActor());
		w_weapon_actor = new_equip_weapon_actor;
	}
}

