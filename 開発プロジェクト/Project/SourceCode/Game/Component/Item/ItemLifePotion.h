#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Item.h"

/* --- �錾 ---- */

class ItemLifePotion : public Item
{
public:

	ItemLifePotion(int32_t recover_value);
	~ItemLifePotion() override = default;

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender() override;

	bool CanConsume(const std::shared_ptr<Actor>& actor) override;

	/// <summary>
	/// �A�C�e�����g�p���A�v���C���[�Ɍ��ʂ�^����
	/// </summary>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor);
	///// <summary>
	///// �A�C�e���𓊝����A���������A�N�^�Ɍ��ʂ�^����
	///// </summary>
	//bool Through() {}
	///// <summary>
	///// ����ɃA�C�e���̌��ʂ�t�^����
	///// </summary>
	//bool Enchant(const std::shared_ptr<Actor>& dst) {}
	/// <summary>
	/// ���̃A�C�e���̃f�[�^���f�[�^�x�[�X����Q�Ƃ���
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	int32_t recover_value; // �񕜗�
};
