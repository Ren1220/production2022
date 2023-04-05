#include "keyboard.h"

/* --- �֐��̒�` ---- */

Keyboard::Keyboard()
{
	// �L�[�{�[�h���͏�Ԃ�ۑ�����o�b�t�@�̏�����
	ZeroMemory(previous_keystates, sizeof(previous_keystates));
	ZeroMemory(current_keystates, sizeof(current_keystates));
}

void Keyboard::Update()
{
	// �O�̃L�[�̏�Ԃ��o�b�t�@�ɕۑ�����
	memcpy_s(previous_keystates, sizeof(previous_keystates), current_keystates, sizeof(current_keystates));

	// ���݂̃L�[�̏�Ԃ��o�b�t�@�ɕۑ�����
	if(GetKeyboardState(current_keystates)) {}
}

Keyboard::KeyState Keyboard::GetKeyState(VK vk) const
{
	unsigned int keystate{ 0x0000 };
	unsigned char key{ static_cast<unsigned char>(vk) };

	// �L�[��������Ă��邩
	if (current_keystates[key] & 0x80)
	{
		keystate |= KeyState::On;

		// �L�[���������u�Ԃł��邩
		if (previous_keystates[key] < 0x80)
		{
			keystate |= KeyState::Down;
		}
	}

	if (current_keystates[key] & 0x01)
	{
		keystate |= KeyState::Toggle;
	}

	if (current_keystates[key] < 0x80)
	{
		keystate |= KeyState::Off;

		// �L�[�𗣂����u�Ԃł��邩
		if(previous_keystates[key] & 0x80)
		{
			keystate |= KeyState::Up;
		}
	}

	return static_cast<KeyState>(keystate);
}