#include "Inventory.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Input/Input.h"
#include "Utility/UserFunc.h"

#include "Audio/AudioHandle.h"

#include "../Actor/Actor.h"
#include "../Component/Item/Item.h"
#include "../Component/Weapon/WeaponHandler.h"

#include "GameSystem.h"
#include "Tutorial.h"
#include "PlayerController.h"

/* --- 定義 ---- */

void Inventory::Initialize()
{
	select_item = 0;
	items = {};
}

void Inventory::Update()
{
	auto& gamepad{ Input::GetInstance().GetGamePad() };
	const auto& player{ GameSystem::GetInstance().GetPlayerController()->GetPlayer() };

	// アイテムを一つも所持していない場合は処理を行わない。
	if (items.empty()) return;

	if (Input::GetInstance().GetKeyboard().GetKeyState(VK::V) & Keyboard::Down)
	{
		// TODO : 実装するか未定、今思いついたけど、アイテムを捨てる操作と合体させるのもあり
		ThrowSelectItem();
	}
	else if (gamepad.GetButton(GamePad::DPAD_RIGHT, GamePad::DOWN))
	{
		++select_item;//RotateInventory(-1); // 右にずらす
	}
	else if (gamepad.GetButton(GamePad::DPAD_LEFT, GamePad::DOWN))
	{
		--select_item;
		//RotateInventory(1); // 左にずらす
	}

	// アイテムを消費・切り替えした場合インデックスが範囲を外れている可能性があるので整える
	AdjustSelectItem();
}

void Inventory::ImGuiRender()
{


	ImGui::Begin("Inventory");

	for (size_t i = 0; i < items.size(); ++i)
	{
		if (items[i] == nullptr) continue;

		ImGui::Text(items[i]->GetName().c_str());

		if (i == select_item)
		{
			// 選択中のアイテムの目印
			ImGui::SameLine();
			ImGui::Text("<--");
		}
	}

	ImGui::End();
}

void Inventory::AddItem(const std::shared_ptr<Actor>& item)
{
	const size_t item_group = std::hash<std::string>()("Item");

	_ASSERT_EXPR(
		std::hash<std::string>()(item->GetGroupName()) == item_group,
		L"インベントリに追加するアクタのグループ名がItemでない");

	// ポインタをそのまま保持するため、再び"Item"グループで拾われないようにするため（座標データを参照して）
	// グループ名を変更する
	item->SetGroupName("InventoryItem");

	// 先頭から最も近い空きの要素にアイテムを追加する
	for (auto& i : items)
	{
		if (i == nullptr)
		{
			i = item;
			break;
		}
	}

}

bool Inventory::IsInventorySpace() const
{
	// 一つでもnullptrの要素があるか
	return std::ranges::any_of(items, [](const auto& i) {return i == nullptr; });
}

void Inventory::ConsumeSelectItem(const std::shared_ptr<Actor>& actor)
{
	if (items.empty()) return;
	// アイテムを使用
	if (GetItem(select_item)->GetComponent<Item>()->Consume(actor))
	{
		// 使用できた場合アイテムを削除
		RemoveSelectItem();

		// サウンドを再生
		AudioHandle attack;
		attack.Play(L"Data/Audio/SE/魔王魂/魔王魂 効果音 飲む01.wav", 0.3f);
		attack.ReverbEffect();

		// チュートリアル完了フラグを立てる
		if (Tutorial::GetInstance().IsTutorial())
		{
			Tutorial::GetInstance().SetFlag(Tutorial::ShowConsumeText, Tutorial::AlreadyDisplayed);
		}
	}
}

void Inventory::ThrowSelectItem()
{
	// TODO : 未実装
}

void Inventory::EnchantSelectItem(const std::shared_ptr<Actor>& actor)
{
	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
	{
		if (auto weapon_actor = weapon_handler->GetWeaponActor())
		{
			// 武器に効果を付与
			if (GetItem(select_item)->GetComponent<Item>()->Enchant(weapon_actor))
			{
				// 使用できた場合アイテムを削除
				RemoveSelectItem();

				// サウンドを再生
				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/On-Jin/刀剣・抜刀02-converted.wav", 0.3f);
				attack.ReverbEffect();

				// チュートリアル完了フラグを立てる
				if (Tutorial::GetInstance().IsTutorial())
				{
					Tutorial::GetInstance().SetFlag(Tutorial::ShowEnchantText, Tutorial::AlreadyDisplayed);
				}
			}
		}
	}
}

void Inventory::RemoveSelectItem()
{
	// 選択中のインデックスの位置のイテレータを除外
	items[select_item].reset();

	// 空きを詰める
	std::ranges::sort(items, [](const auto& lhs, const auto& rhs) { return rhs == nullptr; });
}

void Inventory::AdjustSelectItem()
{
	// 範囲を超えたら最前列（最後列）に
	int64_t i = static_cast<int64_t>(select_item); // unsignedではマイナスにならず正しくループできない

	int64_t c = std::ranges::count_if(items, [](const auto& i) {return i != nullptr; });
	

	select_item = static_cast<size_t>(UserFunc::Loop(i, 0LL, (std::max)(c - 1LL, 0LL)));
	//select_item = static_cast<size_t>(UserFunc::Loop(i, 0LL, static_cast<int64_t>(items.size()) - 1LL));
}

void Inventory::RotateInventory(int rot_left_count)
{
	if (rot_left_count > 0)
	{
		std::rotate(items.begin(), items.begin() + rot_left_count, items.end());
	}
	else
	{
		std::rotate(items.rbegin(), items.rbegin() - rot_left_count, items.rend());
	}

	// 空きを詰める
	std::ranges::sort(items, [](const auto& lhs, const auto& rhs) { return rhs == nullptr; });
}


