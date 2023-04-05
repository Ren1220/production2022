#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Item.h"

#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- �錾 ---- */

class ItemSparkPotion : public Item
{
public:

	ItemSparkPotion() : effect_handle() {}
	virtual ~ItemSparkPotion() override = default;

	void Start() override;
	void Update(float elapsed_time) override;
	void End() override;

	/// <summary>
	/// �G���`�����g�\��
	/// </summary>
	bool CanEnchant(const std::shared_ptr<Actor>& opponent_actor) override;

	/// <summary>
	/// �A�C�e�����g�p���A�v���C���[�Ɍ��ʂ�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	bool Consume(const std::shared_ptr<Actor>& opponent_actor) override { return false; }
	/// <summary>
	/// �A�C�e���𓊝����A���������A�N�^�Ɍ��ʂ�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	bool Through() override { return false; }
	/// <summary>
	/// ����ɃA�C�e���̌��ʂ�t�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	bool Enchant(const std::shared_ptr<Actor>& weapon_actor) override;
	/// <summary>
	/// ���̃A�C�e���̃f�[�^���f�[�^�x�[�X����Q�Ƃ���
	/// </summary>
	const ItemDataBase::Data& GetItemData() override;

private:

	EffekseerHandle effect_handle;

};
