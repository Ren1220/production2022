#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <unordered_set>

#include "../Component.h"

#include "Enchant/WeaponEnchant.h"

//#include "Effect/EffectBase.h"

struct GLTFModelData::Node;

/* --- �錾 ---- */

/// <summary>
/// mixamorig��WeaponHand�����݂��Ȃ��ꍇ�g�p�ł��Ȃ��B�����Őݒ肷�邱�ƁB
/// �����āA�e�A�N�^��WeaponHandlerComponent��ǉ����邱�ƁB
/// </summary>
class Weapon : public Component
{
public:

	/// <summary>
	/// ���̃A�N�^�ɃA�N�^�𑕔������Ȃ�
	/// </summary>
	/// <param name="attack_power">����U����</param>
	/// <param name="durability">����ϋv�l</param>
	Weapon(int32_t attack_power, int32_t durability);

	~Weapon() override = default;

	/// <summary>
	/// �J�n����
	/// </summary>
	void Start() override;
	void End() override;
	
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void Update(float elapsed_time) override;

	/// <summary>
	/// �������̏���
	/// </summary>
	/// <param name="opponent"></param>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent) override;

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender() override;

	/// <summary>
	/// �������O���BWeaponHander���Ŏg�p�B
	/// <para>�������Ă���A�N�^�̎Q�Ƃ�؂�</para>
	/// </summary>
	void Unequip();

	/// <summary>
	/// ��������BWeaponHander���Ŏg�p�B
	/// <para>�����̃A�N�^���Q�Ƃ���</para>
	/// </summary>
	void Equip(const std::shared_ptr<Actor>& equip_actor);

	void CollideCombo0();
	void CollideCombo1();
	void CollideCombo2();
	void CollideSlash();
	void CollideJumpATK();

	void CollideCapsule(float length, float radius, const Vector3& offset = {});
	void CollideSphere(float radius, const Vector3& offset = {});
	void NoCollide();

	/// <summary>
	/// ���g�𑕔����Ă���e�A�N�^�����݂��Ă���ꍇ�A
	/// ���̕�����������Ă���W���C���g�̃|�C���^���擾����
	/// </summary>
	/// <returns></returns>
	//std::shared_ptr<GLTFModelData::Node> GetParentWeaponJointNode();

	/// <summary>
	/// ����̃g���C�����s��
	/// </summary>
	/// <param name="trail_texture_path">�g���C���e�N�X�`��</param>
	void CreateTrail(const wchar_t* trail_texture_path);

	void SetWeaponEnchant(std::unique_ptr<WeaponEnchant>&& enchant = nullptr);
	const std::unique_ptr<WeaponEnchant>& GetWeaponEnchant() { return weapon_enchant; }

	//std::shared_ptr<EffectBase> GetTrailEffect() const;

	void SubDurability(int32_t val);

private:

	/// <summary>
	/// ��������Ă����Ԃł̂ӂ�܂�
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateIsEquiped(float elapsed_time);
	/// <summary>
	/// ��������Ă��Ȃ���Ԃł̂ӂ�܂�
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdeteIsNotEquiped(float elapsed_time);
	/// <summary>
	/// ��̋�Ԃ�e�A�N�^�̕���W���C���g��ԂɍX�V����
	/// </summary>
	void UpdateTransformToParentWeaponJoint(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node);
	/// <summary>
	/// �U���ΏۂƂ̌���
	/// </summary>
	void CollideToAttackTarget(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node);

	void AttackByPlayer(const std::shared_ptr<Actor>& owner, const std::shared_ptr<Actor>& target);

private:

	/// <summary>
	/// ���̕�������A�N�^
	/// </summary>
	std::shared_ptr<Actor> GetOwner() const;

	bool is_equiped; // ���̃A�N�^�ɑ�������Ă����Ԃ�

	bool can_weapon_collide{ false }; // ���킪������s���邩
	float collide_length;

	std::unique_ptr<WeaponEnchant> weapon_enchant;
	bool is_reduced; // �G���`�����g��������ōU�����A���̑ϋv�����̃t���[���ŏ������

	std::string attack_target_name; // �U���Ώ�

	int32_t attack_power; // �U����
	int32_t durability; // �ϋv�l

	// ��x���������G�ɓ����U����������Ȃ��悤�ɂ��邽�߂Ɏg��
	std::unordered_set<std::string> hit_targets;

	//std::weak_ptr<EffectBase> w_trail_effect; // ����̃g���C���̊Ǘ�

#ifdef _DEBUG
	int select_enchant = 0;
#endif // !_DEBUG


};
