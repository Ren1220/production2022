#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Item.h"

#include "../../General/GameSystem.h"

/* --- 宣言 ---- */

class ItemPoisonPotion : public Item
{
public:

	ItemPoisonPotion(int32_t damage_value);
	~ItemPoisonPotion() override = default;

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;

	/// <summary>
	/// 使用可能か
	/// </summary>
	bool CanConsume(const std::shared_ptr<Actor>& opponent_actor) override;

	/// <summary>
	/// エンチャント可能か
	/// </summary>
	bool CanEnchant(const std::shared_ptr<Actor>& opponent_actor) override;

	/// <summary>
	/// アイテムを使用し、プレイヤーに効果を与える
	/// </summary>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor);
	///// <summary>
	///// アイテムを投擲し、命中したアクタに効果を与える
	///// </summary>
	//bool Through() {}
	/// <summary>
	/// 武器にアイテムの効果を付与する
	/// </summary>
	bool Enchant(const std::shared_ptr<Actor>& weapon_actor);
	/// <summary>
	/// このアイテムのデータをデータベースから参照する
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	int32_t damage_value; // ダメージ量
};
