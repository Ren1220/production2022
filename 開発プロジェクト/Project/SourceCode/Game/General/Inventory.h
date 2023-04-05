#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <array>

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class Inventory
{
public:

	Inventory()
		: items(), select_item(0ULL) {}
	~Inventory() = default;

	void Initialize();

	void Update();

	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender();

	/// <summary>
	/// �A�C�e�����C���x���g���ɒǉ�
	/// </summary>
	/// <param name="item">�ǉ�����A�C�e��</param>
	void AddItem(const std::shared_ptr<Actor>& item);

	/// <summary>
	/// �C���x���g���ɋ󂫂����邩
	/// </summary>
	bool IsInventorySpace() const;

	/// <summary>
	/// �����ł���A�C�e���̏��
	/// </summary>
	/// <returns></returns>
	static constexpr size_t GetInventorySize() { return inventory_size; }


	const std::shared_ptr<Actor>& GetItem(size_t i) const { return items[i]; }
	std::shared_ptr<Actor> GetSelectItem() const { return items.empty() ? nullptr : items[select_item]; }
	size_t GetSelectItemIndex() const { return select_item; }

	/// <summary>
	/// �I�𒆂̃A�C�e��������A�����̃A�N�^�Ɍ��ʂ�^����B
	/// </summary>
	void ConsumeSelectItem(const std::shared_ptr<Actor>& actor);
	/// <summary>
	/// �I�𒆂̃A�C�e���𓊝�����B (������)
	/// </summary>
	void ThrowSelectItem();
	/// <summary>
	/// �I�𒆂̃A�C�e��������A�����̃A�N�^�̑������镐��Ɍ��ʂ�^����B
	/// </summary>
	void EnchantSelectItem(const std::shared_ptr<Actor>& actor);


private:

	/// <summary>
	/// �I�𒆂̃A�C�e�����A�C�e���z�񂩂珜�O����
	/// </summary>
	void RemoveSelectItem();

	/// <summary>
	/// �I�𒆂̃A�C�e���C���f�b�N�X�����X�g�͈͓̔��Ɏ��߂�
	/// </summary>
	void AdjustSelectItem();

	void RotateInventory(int rot_left_count);

private:

	static constexpr size_t inventory_size{ 5ULL };

	std::array<std::shared_ptr<Actor>, inventory_size> items;
	size_t select_item;

};
