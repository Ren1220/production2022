#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Item.h"

#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- 宣言 ---- */

class ItemFirePotion : public Item
{
public:

	ItemFirePotion() : effect_handle() {};
	virtual ~ItemFirePotion() override = default;

	void Start() override;
	void Update(float elapsed_time) override;
	void End() override;

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
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor) override { return false; }

	/// <summary>
	/// アイテムを投擲し、命中したアクタに効果を与える
	/// </summary>
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	bool Through() override { return false; }

	/// <summary>
	/// 武器にアイテムの効果を付与する
	/// </summary>
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	bool Enchant(const std::shared_ptr<Actor>& weapon_actor) override;
	/// <summary>
	/// このアイテムのデータをデータベースから参照する
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	EffekseerHandle effect_handle;

};
