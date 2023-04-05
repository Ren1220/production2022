#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFModelData.h"
#include "../../Component/Component.h"

/* --- 宣言 ---- */

class WeaponHandler : public Component
{
public:

	WeaponHandler()
		: dura_red_reflection(1.0f) {}
	~WeaponHandler() override = default;

	void Start() override;

	/// <summary>
	/// 武器アクタを設定
	/// </summary>
	void SetWeaponActor(const std::shared_ptr<Actor>& weapon_actor);

	std::shared_ptr<Actor> GetWeaponActor() const;

	/// <summary>
	/// 武器を持つノードの情報
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<GLTFModelData::Node> GetWeaponJointNode();

	/// <summary>
	/// 現在装備している武器があればフィールドに捨て、新しく引数の武器を装備する。
	/// 引数にnullptrを設定すると、装備を解除する操作のみを行う
	/// </summary>
	void ChangeWeapon(const std::shared_ptr<Actor>& new_equip_weapon_actor);

	/// <summary>
	/// 武器耐久減少反映率
	/// </summary>
	float GetDuraRedReflection() const { return dura_red_reflection; }

	void SetDuraRedReflection(float ratio) { dura_red_reflection = ratio; }

private:

	std::weak_ptr<Actor> w_weapon_actor; // 武器アクタ
	std::weak_ptr<GLTFModelData::Node> w_weapon_joint_node; // 武器を持つノードの

	float dura_red_reflection; // 武器耐久減少反映率
};
