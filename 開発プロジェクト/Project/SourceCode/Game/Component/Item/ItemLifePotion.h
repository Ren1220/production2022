#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Item.h"

/* --- 宣言 ---- */

class ItemLifePotion : public Item
{
public:

	ItemLifePotion(int32_t recover_value);
	~ItemLifePotion() override = default;

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;

	bool CanConsume(const std::shared_ptr<Actor>& actor) override;

	/// <summary>
	/// アイテムを使用し、プレイヤーに効果を与える
	/// </summary>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor);
	///// <summary>
	///// アイテムを投擲し、命中したアクタに効果を与える
	///// </summary>
	//bool Through() {}
	///// <summary>
	///// 武器にアイテムの効果を付与する
	///// </summary>
	//bool Enchant(const std::shared_ptr<Actor>& dst) {}
	/// <summary>
	/// このアイテムのデータをデータベースから参照する
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	int32_t recover_value; // 回復量
};
