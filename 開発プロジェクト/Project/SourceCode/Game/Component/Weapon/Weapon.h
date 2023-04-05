#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <unordered_set>

#include "../Component.h"

#include "Enchant/WeaponEnchant.h"

//#include "Effect/EffectBase.h"

struct GLTFModelData::Node;

/* --- 宣言 ---- */

/// <summary>
/// mixamorigにWeaponHandが存在しない場合使用できない。自分で設定すること。
/// 加えて、親アクタにWeaponHandlerComponentを追加すること。
/// </summary>
class Weapon : public Component
{
public:

	/// <summary>
	/// 他のアクタにアクタを装備させない
	/// </summary>
	/// <param name="attack_power">武器攻撃力</param>
	/// <param name="durability">武器耐久値</param>
	Weapon(int32_t attack_power, int32_t durability);

	~Weapon() override = default;

	/// <summary>
	/// 開始処理
	/// </summary>
	void Start() override;
	void End() override;
	
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Update(float elapsed_time) override;

	/// <summary>
	/// 交差時の処理
	/// </summary>
	/// <param name="opponent"></param>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent) override;

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;

	/// <summary>
	/// 装備を外す。WeaponHander内で使用。
	/// <para>装備しているアクタの参照を切る</para>
	/// </summary>
	void Unequip();

	/// <summary>
	/// 装備する。WeaponHander内で使用。
	/// <para>引数のアクタを参照する</para>
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
	/// 自身を装備している親アクタが存在している場合、
	/// その武器を所持しているジョイントのポインタを取得する
	/// </summary>
	/// <returns></returns>
	//std::shared_ptr<GLTFModelData::Node> GetParentWeaponJointNode();

	/// <summary>
	/// 武器のトレイルを行う
	/// </summary>
	/// <param name="trail_texture_path">トレイルテクスチャ</param>
	void CreateTrail(const wchar_t* trail_texture_path);

	void SetWeaponEnchant(std::unique_ptr<WeaponEnchant>&& enchant = nullptr);
	const std::unique_ptr<WeaponEnchant>& GetWeaponEnchant() { return weapon_enchant; }

	//std::shared_ptr<EffectBase> GetTrailEffect() const;

	void SubDurability(int32_t val);

private:

	/// <summary>
	/// 装備されている状態でのふるまい
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateIsEquiped(float elapsed_time);
	/// <summary>
	/// 装備されていない状態でのふるまい
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdeteIsNotEquiped(float elapsed_time);
	/// <summary>
	/// 基準の空間を親アクタの武器ジョイント空間に更新する
	/// </summary>
	void UpdateTransformToParentWeaponJoint(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node);
	/// <summary>
	/// 攻撃対象との交差
	/// </summary>
	void CollideToAttackTarget(const std::shared_ptr<GLTFModelData::Node>& weapon_joint_node);

	void AttackByPlayer(const std::shared_ptr<Actor>& owner, const std::shared_ptr<Actor>& target);

private:

	/// <summary>
	/// この武器を持つアクタ
	/// </summary>
	std::shared_ptr<Actor> GetOwner() const;

	bool is_equiped; // 他のアクタに装備されている状態か

	bool can_weapon_collide{ false }; // 武器が判定を行えるか
	float collide_length;

	std::unique_ptr<WeaponEnchant> weapon_enchant;
	bool is_reduced; // エンチャントした武器で攻撃し、その耐久をこのフレームで消費したか

	std::string attack_target_name; // 攻撃対象

	int32_t attack_power; // 攻撃力
	int32_t durability; // 耐久値

	// 一度命中した敵に同じ攻撃が当たらないようにするために使う
	std::unordered_set<std::string> hit_targets;

	//std::weak_ptr<EffectBase> w_trail_effect; // 武器のトレイルの管理

#ifdef _DEBUG
	int select_enchant = 0;
#endif // !_DEBUG


};
