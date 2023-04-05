#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <windows.h>
#include "keylist.h"

/* --- 宣言 ---- */

class Keyboard
{
public:

	enum KeyState : unsigned int
	{
		Off		= 0x00001,	// 押されていない
		On		= 0x00010,	// 押している
		Down	= 0x00100,	// 押した瞬間
		Up		= 0x01000,	// 離した瞬間
		Toggle	= 0x10000	// トグルしているか 
	};

	Keyboard();
	~Keyboard() = default;

	void Update();
	
	KeyState GetKeyState(VK vk) const;


private:
	
	BYTE previous_keystates[256];
	BYTE current_keystates[256];
};
