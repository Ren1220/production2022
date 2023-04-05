#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>

#include "Utility/Singleton.h"
#include "Utility/Trits.h"

/* --- �錾 ---- */

class Tutorial : public Singleton<Tutorial>
{
public:

	enum Flag : uint8_t
	{
		ShowAttackText , // �ʏ�U���̃`���[�g���A��
		ShowComboText ,  // �R���{�̃`���[�g���A��
		ShowSlashText  , // ���U��
		ShowDodgeText  , // ����s��
		ShowCounterText, // �����i�W�����v�U���j
		ShowEnchantText, // �G���`�����g
		ShowEnchantText2, // �G���`�����g(�ǉ��̃e�L�X�g)
		ShowConsumeText, // ����
		ElementCount,
	};

	enum State : uint8_t
	{
		NotDisplayed,		// ���\��
		Displaying,			// �\����
		AlreadyDisplayed,	// �\���ς�
	};

	Tutorial()
		: flags(), is_tutorial(false) {}
	~Tutorial() = default;

	void ResetFlag()
	{
		flags = {};
	}

	/// <summary>
	/// �t���O�̐ݒ�
	/// </summary>
	/// <param name="f"></param>
	/// <param name="t">0 ���\��, 1 �\����, 2 �\���ς�</param>
	void SetFlag(Flag f, State t)
	{
		flags.Set(f, t);
	}

	/// <summary>
	/// �t���O�̎擾
	/// </summary>
	/// <returns>0 ���\��, 1 �\����, 2 �\���ς�</returns>
	State GetFlag(Flag f) const
	{
		return static_cast<State>(flags[f]);
	}

	bool IsTutorial() const { return is_tutorial; }
	void SetTutorial(bool b) { is_tutorial = b; }

private:

	bool is_tutorial;

	Trits<ElementCount> flags;
};
