#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <array>
#include <memory>

#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

#include "../../../Actor/Actor.h"
#include "../../../Actor/ActorPlayer.h"
#include "../../../Behavior/StateMachine/StateMachine.h"

/* --- �O���錾 ---- */

class Weapon;

/* --- �錾 ---- */

class WeaponEnchant
{
public:

	WeaponEnchant()
		: usuable_count(20) {}
	virtual ~WeaponEnchant();

	/// <summary>
	/// �G�t�F�N�g�̍Đ��������ōs����
	/// </summary>
	void Initialize(const Vector3& position);

	/// <summary>
	/// �e�t���A�Ő������ꂽ�I�u�W�F�N�g�́A���̃|�C���^���e�X�e�[�W���ێ����Ă��邽�߁A
	/// �A�N�^�}�l�[�W���[������j������Ă��f�X�g���N�^���Ă΂�Ȃ��B
	/// </summary>
	void EndProcess();

	// ���o����
	virtual void ShowWeaponEffect();
	virtual void HideWeaponEffect();

	void Attack(
		const std::shared_ptr<Component>& weapon, 
		const std::shared_ptr<Actor>& target, 
		int32_t weapon_base_damage) 
	{
		_ASSERT_EXPR(0, L"������");
	};

	void AttackByPlayer(
		const std::shared_ptr<Actor>& player,
		const std::shared_ptr<Actor>& target,
		int32_t weapon_base_damage);

	virtual void PlaySlashTrail(const std::shared_ptr<Actor>& actor){};

	virtual void CollideCombo0(const std::shared_ptr<Weapon>& weapon);
	virtual void CollideCombo1(const std::shared_ptr<Weapon>& weapon);
	virtual void CollideCombo2(const std::shared_ptr<Weapon>& weapon);
	virtual void CollideSlash(const std::shared_ptr<Weapon>& weapon);
	virtual void CollideJumpAtk(const std::shared_ptr<Weapon>& weapon);

	virtual const char* GetWeaponEffectPath() const { return nullptr; }
	
	const std::unique_ptr<EffekseerHandle>& GetWeaponEffect() const { return weapon_effect; }

	int32_t GetUsuableCount() const { return usuable_count; }
	void SubUsuableCount(int32_t c) { usuable_count -= c; }

protected:

	virtual void AttackCombo0(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) = 0;
	virtual void AttackCombo1(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) = 0;
	virtual void AttackCombo2(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) = 0;
	virtual void AttackSlash(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) = 0;
	virtual void AttackJumpAtk(const std::shared_ptr<Actor>& player, const std::shared_ptr<Actor>& target, int32_t weapon_base_damage) = 0;

	std::unique_ptr<EffekseerHandle> weapon_effect;

	int32_t usuable_count;
};

