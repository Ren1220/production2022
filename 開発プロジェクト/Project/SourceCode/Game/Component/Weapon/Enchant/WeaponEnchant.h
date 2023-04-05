#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <array>
#include <memory>

#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

#include "../../../Actor/Actor.h"
#include "../../../Actor/ActorPlayer.h"
#include "../../../Behavior/StateMachine/StateMachine.h"

/* --- 前方宣言 ---- */

class Weapon;

/* --- 宣言 ---- */

class WeaponEnchant
{
public:

	WeaponEnchant()
		: usuable_count(20) {}
	virtual ~WeaponEnchant();

	/// <summary>
	/// エフェクトの再生もここで行われる
	/// </summary>
	void Initialize(const Vector3& position);

	/// <summary>
	/// 各フロアで生成されたオブジェクトは、そのポインタを各ステージが保持しているため、
	/// アクタマネージャー内から破棄されてもデストラクタが呼ばれない。
	/// </summary>
	void EndProcess();

	// 視覚効果
	virtual void ShowWeaponEffect();
	virtual void HideWeaponEffect();

	void Attack(
		const std::shared_ptr<Component>& weapon, 
		const std::shared_ptr<Actor>& target, 
		int32_t weapon_base_damage) 
	{
		_ASSERT_EXPR(0, L"未実装");
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

