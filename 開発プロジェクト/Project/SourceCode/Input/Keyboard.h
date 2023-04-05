#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <windows.h>
#include "keylist.h"

/* --- �錾 ---- */

class Keyboard
{
public:

	enum KeyState : unsigned int
	{
		Off		= 0x00001,	// ������Ă��Ȃ�
		On		= 0x00010,	// �����Ă���
		Down	= 0x00100,	// �������u��
		Up		= 0x01000,	// �������u��
		Toggle	= 0x10000	// �g�O�����Ă��邩 
	};

	Keyboard();
	~Keyboard() = default;

	void Update();
	
	KeyState GetKeyState(VK vk) const;


private:
	
	BYTE previous_keystates[256];
	BYTE current_keystates[256];
};
