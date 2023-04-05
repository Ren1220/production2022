#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <array>

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class Inventory
{
public:

	Inventory()
		: items(), select_item(0ULL) {}
	~Inventory() = default;

	void Initialize();

	void Update();

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender();

	/// <summary>
	/// アイテムをインベントリに追加
	/// </summary>
	/// <param name="item">追加するアイテム</param>
	void AddItem(const std::shared_ptr<Actor>& item);

	/// <summary>
	/// インベントリに空きがあるか
	/// </summary>
	bool IsInventorySpace() const;

	/// <summary>
	/// 所持できるアイテムの上限
	/// </summary>
	/// <returns></returns>
	static constexpr size_t GetInventorySize() { return inventory_size; }


	const std::shared_ptr<Actor>& GetItem(size_t i) const { return items[i]; }
	std::shared_ptr<Actor> GetSelectItem() const { return items.empty() ? nullptr : items[select_item]; }
	size_t GetSelectItemIndex() const { return select_item; }

	/// <summary>
	/// 選択中のアイテムを消費し、引数のアクタに効果を与える。
	/// </summary>
	void ConsumeSelectItem(const std::shared_ptr<Actor>& actor);
	/// <summary>
	/// 選択中のアイテムを投擲する。 (未実装)
	/// </summary>
	void ThrowSelectItem();
	/// <summary>
	/// 選択中のアイテムを消費し、引数のアクタの装備する武器に効果を与える。
	/// </summary>
	void EnchantSelectItem(const std::shared_ptr<Actor>& actor);


private:

	/// <summary>
	/// 選択中のアイテムをアイテム配列から除外する
	/// </summary>
	void RemoveSelectItem();

	/// <summary>
	/// 選択中のアイテムインデックスをリストの範囲内に収める
	/// </summary>
	void AdjustSelectItem();

	void RotateInventory(int rot_left_count);

private:

	static constexpr size_t inventory_size{ 5ULL };

	std::array<std::shared_ptr<Actor>, inventory_size> items;
	size_t select_item;

};
