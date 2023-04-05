#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Item.h"

#include "../../General/GameSystem.h"

/* --- �錾 ---- */

class ItemPoisonPotion : public Item
{
public:

	ItemPoisonPotion(int32_t damage_value);
	~ItemPoisonPotion() override = default;

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender() override;

	/// <summary>
	/// �g�p�\��
	/// </summary>
	bool CanConsume(const std::shared_ptr<Actor>& opponent_actor) override;

	/// <summary>
	/// �G���`�����g�\��
	/// </summary>
	bool CanEnchant(const std::shared_ptr<Actor>& opponent_actor) override;

	/// <summary>
	/// �A�C�e�����g�p���A�v���C���[�Ɍ��ʂ�^����
	/// </summary>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor);
	///// <summary>
	///// �A�C�e���𓊝����A���������A�N�^�Ɍ��ʂ�^����
	///// </summary>
	//bool Through() {}
	/// <summary>
	/// ����ɃA�C�e���̌��ʂ�t�^����
	/// </summary>
	bool Enchant(const std::shared_ptr<Actor>& weapon_actor);
	/// <summary>
	/// ���̃A�C�e���̃f�[�^���f�[�^�x�[�X����Q�Ƃ���
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	int32_t damage_value; // �_���[�W��
};
