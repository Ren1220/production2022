#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Component.h"
#include "../Item/ItemDataBase.h"

/* --- �錾 ---- */

/// <summary>
/// �A�C�e���R���|�[�l���g�̊��N���X
/// </summary>
class Item : public Component
{
public:

	Item() = default;
	virtual ~Item() override = default;

	/// <summary>
	/// �J�n����
	/// </summary>
	virtual void Start() override {};

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	virtual void Update(float elapsed_time) override {};

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	virtual void ImGuiRender() override {};

	/// <summary>
	/// �������Ɏ��s����֐�
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override {};

	/// <summary>
	/// �g�p�\��
	/// </summary>
	virtual bool CanConsume(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// �����\���i�������������j
	/// </summary>
	virtual bool CanThrough(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// �G���`�����g�\��
	/// </summary>
	virtual bool CanEnchant(const std::shared_ptr<Actor>& opponent_actor) { return false; };

	/// <summary>
	/// �A�C�e�����g�p���A�v���C���[�Ɍ��ʂ�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	virtual bool Consume(const std::shared_ptr<Actor>& dst) { return false; }

	/// <summary>
	/// �A�C�e���𓊝����A���������A�N�^�Ɍ��ʂ�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	virtual bool Through() { return false; }

	/// <summary>
	/// ����ɃA�C�e���̌��ʂ�t�^����
	/// </summary>
	/// <returns>true�̎��������A�C�e�����g�p�����B</returns>
	virtual bool Enchant(const std::shared_ptr<Actor>& dst) { return false; }

	/// <summary>
	/// ���̃A�C�e���̃f�[�^���f�[�^�x�[�X����Q�Ƃ���
	/// </summary>
	virtual const ItemDataBase::Data& GetItemData() = 0;

protected:

	/// <summary>
	/// �Ώۂ�����������Ă��邩
	/// </summary>
	/// <param name="dst"></param>
	/// <returns></returns>
	bool HasWeapon(const std::shared_ptr<Actor>& dst) const;

};
