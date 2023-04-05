#include "ItemLifePotion.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../Actor/Actor.h"
#include "../Item/ItemDataBase.h"
#include "../Health/Health.h"
#include "../../General/GameSystem.h"

/* --- 定義 ---- */

ItemLifePotion::ItemLifePotion(int32_t recover_value)
	:recover_value(recover_value)
{
}

void ItemLifePotion::ImGuiRender()
{
	ImGui::SliderInt(TO_CHAR(u8"回復量"), &recover_value, -10000, 10000);
}

bool ItemLifePotion::CanConsume(const std::shared_ptr<Actor>& actor)
{
	return true;
}

bool ItemLifePotion::Consume(const std::shared_ptr<Actor>& opponent_actor)
{
	if (auto health = opponent_actor->GetComponent<Health>())
	{
		health->Recover(recover_value);
		return true;
	}

	return false;
}

const ItemDataBase::Data& ItemLifePotion::GetItemData()
{
	return GameSystem::GetInstance().GetItemDataBase()->GetData(ItemDataBase::List::LifePotion);
}
