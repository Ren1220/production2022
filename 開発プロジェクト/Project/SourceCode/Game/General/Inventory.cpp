#include "Inventory.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Input/Input.h"
#include "Utility/UserFunc.h"

#include "Audio/AudioHandle.h"

#include "../Actor/Actor.h"
#include "../Component/Item/Item.h"
#include "../Component/Weapon/WeaponHandler.h"

#include "GameSystem.h"
#include "Tutorial.h"
#include "PlayerController.h"

/* --- ��` ---- */

void Inventory::Initialize()
{
	select_item = 0;
	items = {};
}

void Inventory::Update()
{
	auto& gamepad{ Input::GetInstance().GetGamePad() };
	const auto& player{ GameSystem::GetInstance().GetPlayerController()->GetPlayer() };

	// �A�C�e��������������Ă��Ȃ��ꍇ�͏������s��Ȃ��B
	if (items.empty()) return;

	if (Input::GetInstance().GetKeyboard().GetKeyState(VK::V) & Keyboard::Down)
	{
		// TODO : �������邩����A���v���������ǁA�A�C�e�����̂Ă鑀��ƍ��̂�����̂�����
		ThrowSelectItem();
	}
	else if (gamepad.GetButton(GamePad::DPAD_RIGHT, GamePad::DOWN))
	{
		++select_item;//RotateInventory(-1); // �E�ɂ��炷
	}
	else if (gamepad.GetButton(GamePad::DPAD_LEFT, GamePad::DOWN))
	{
		--select_item;
		//RotateInventory(1); // ���ɂ��炷
	}

	// �A�C�e��������E�؂�ւ������ꍇ�C���f�b�N�X���͈͂��O��Ă���\��������̂Ő�����
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
			// �I�𒆂̃A�C�e���̖ڈ�
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
		L"�C���x���g���ɒǉ�����A�N�^�̃O���[�v����Item�łȂ�");

	// �|�C���^�����̂܂ܕێ����邽�߁A�Ă�"Item"�O���[�v�ŏE���Ȃ��悤�ɂ��邽�߁i���W�f�[�^���Q�Ƃ��āj
	// �O���[�v����ύX����
	item->SetGroupName("InventoryItem");

	// �擪����ł��߂��󂫂̗v�f�ɃA�C�e����ǉ�����
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
	// ��ł�nullptr�̗v�f�����邩
	return std::ranges::any_of(items, [](const auto& i) {return i == nullptr; });
}

void Inventory::ConsumeSelectItem(const std::shared_ptr<Actor>& actor)
{
	if (items.empty()) return;
	// �A�C�e�����g�p
	if (GetItem(select_item)->GetComponent<Item>()->Consume(actor))
	{
		// �g�p�ł����ꍇ�A�C�e�����폜
		RemoveSelectItem();

		// �T�E���h���Đ�
		AudioHandle attack;
		attack.Play(L"Data/Audio/SE/������/������ ���ʉ� ����01.wav", 0.3f);
		attack.ReverbEffect();

		// �`���[�g���A�������t���O�𗧂Ă�
		if (Tutorial::GetInstance().IsTutorial())
		{
			Tutorial::GetInstance().SetFlag(Tutorial::ShowConsumeText, Tutorial::AlreadyDisplayed);
		}
	}
}

void Inventory::ThrowSelectItem()
{
	// TODO : ������
}

void Inventory::EnchantSelectItem(const std::shared_ptr<Actor>& actor)
{
	if (auto weapon_handler = actor->GetComponent<WeaponHandler>())
	{
		if (auto weapon_actor = weapon_handler->GetWeaponActor())
		{
			// ����Ɍ��ʂ�t�^
			if (GetItem(select_item)->GetComponent<Item>()->Enchant(weapon_actor))
			{
				// �g�p�ł����ꍇ�A�C�e�����폜
				RemoveSelectItem();

				// �T�E���h���Đ�
				AudioHandle attack;
				attack.Play(L"Data/Audio/SE/On-Jin/�����E����02-converted.wav", 0.3f);
				attack.ReverbEffect();

				// �`���[�g���A�������t���O�𗧂Ă�
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
	// �I�𒆂̃C���f�b�N�X�̈ʒu�̃C�e���[�^�����O
	items[select_item].reset();

	// �󂫂��l�߂�
	std::ranges::sort(items, [](const auto& lhs, const auto& rhs) { return rhs == nullptr; });
}

void Inventory::AdjustSelectItem()
{
	// �͈͂𒴂�����őO��i�Ō��j��
	int64_t i = static_cast<int64_t>(select_item); // unsigned�ł̓}�C�i�X�ɂȂ炸���������[�v�ł��Ȃ�

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

	// �󂫂��l�߂�
	std::ranges::sort(items, [](const auto& lhs, const auto& rhs) { return rhs == nullptr; });
}


