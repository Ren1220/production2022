#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/Model/GLTFModelData.h"
#include "../../Component/Component.h"

/* --- �錾 ---- */

class WeaponHandler : public Component
{
public:

	WeaponHandler()
		: dura_red_reflection(1.0f) {}
	~WeaponHandler() override = default;

	void Start() override;

	/// <summary>
	/// ����A�N�^��ݒ�
	/// </summary>
	void SetWeaponActor(const std::shared_ptr<Actor>& weapon_actor);

	std::shared_ptr<Actor> GetWeaponActor() const;

	/// <summary>
	/// ��������m�[�h�̏��
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<GLTFModelData::Node> GetWeaponJointNode();

	/// <summary>
	/// ���ݑ������Ă��镐�킪����΃t�B�[���h�Ɏ̂āA�V���������̕���𑕔�����B
	/// ������nullptr��ݒ肷��ƁA�������������鑀��݂̂��s��
	/// </summary>
	void ChangeWeapon(const std::shared_ptr<Actor>& new_equip_weapon_actor);

	/// <summary>
	/// ����ϋv�������f��
	/// </summary>
	float GetDuraRedReflection() const { return dura_red_reflection; }

	void SetDuraRedReflection(float ratio) { dura_red_reflection = ratio; }

private:

	std::weak_ptr<Actor> w_weapon_actor; // ����A�N�^
	std::weak_ptr<GLTFModelData::Node> w_weapon_joint_node; // ��������m�[�h��

	float dura_red_reflection; // ����ϋv�������f��
};
