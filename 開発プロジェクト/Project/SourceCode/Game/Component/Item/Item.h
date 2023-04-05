#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Component.h"
#include "../Item/ItemDataBase.h"

/* --- 宣言 ---- */

/// <summary>
/// アイテムコンポーネントの基底クラス
/// </summary>
class Item : public Component
{
public:

	Item() = default;
	virtual ~Item() override = default;

	/// <summary>
	/// 開始処理
	/// </summary>
	virtual void Start() override {};

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	virtual void Update(float elapsed_time) override {};

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	virtual void ImGuiRender() override {};

	/// <summary>
	/// 交差時に実行する関数
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override {};

	/// <summary>
	/// 使用可能か
	/// </summary>
	virtual bool CanConsume(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// 投擲可能か（投擲が未実装）
	/// </summary>
	virtual bool CanThrough(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// エンチャント可能か
	/// </summary>
	virtual bool CanEnchant(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// アイテムを使用し、プレイヤーに効果を与える
	/// </summary>
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	virtual bool Consume(const std::shared_ptr<Actor>& dst) { return false; }

	/// <summary>
	/// アイテムを投擲し、命中したアクタに効果を与える
	/// </summary>
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	virtual bool Through() { return false; }

	/// <summary>
	/// 武器にアイテムの効果を付与する
	/// </summary>
	/// <returns>trueの時正しくアイテムを使用した。</returns>
	virtual bool Enchant(const std::shared_ptr<Actor>& dst) { return false; }

	/// <summary>
	/// このアイテムのデータをデータベースから参照する
	/// </summary>
	virtual const ItemDataBase::Data& GetItemData() = 0;

protected:

	/// <summary>
	/// 対象が武器を持っているか
	/// </summary>
	/// <param name="dst"></param>
	/// <returns></returns>
	bool HasWeapon(const std::shared_ptr<Actor>& dst) const;

};
