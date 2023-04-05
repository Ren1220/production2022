#include "keyboard.h"

/* --- 関数の定義 ---- */

Keyboard::Keyboard()
{
	// キーボード入力状態を保存するバッファの初期化
	ZeroMemory(previous_keystates, sizeof(previous_keystates));
	ZeroMemory(current_keystates, sizeof(current_keystates));
}

void Keyboard::Update()
{
	// 前のキーの状態をバッファに保存する
	memcpy_s(previous_keystates, sizeof(previous_keystates), current_keystates, sizeof(current_keystates));

	// 現在のキーの状態をバッファに保存する
	if(GetKeyboardState(current_keystates)) {}
}

Keyboard::KeyState Keyboard::GetKeyState(VK vk) const
{
	unsigned int keystate{ 0x0000 };
	unsigned char key{ static_cast<unsigned char>(vk) };

	// キーが押されているか
	if (current_keystates[key] & 0x80)
	{
		keystate |= KeyState::On;

		// キーを押した瞬間であるか
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

		// キーを離した瞬間であるか
		if(previous_keystates[key] & 0x80)
		{
			keystate |= KeyState::Up;
		}
	}

	return static_cast<KeyState>(keystate);
}