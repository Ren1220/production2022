#pragma once

//http://kts.sakaiweb.com/virtualkeycodes.html
//https://hakase0274.hatenablog.com/entry/2019/10/07/200000
//★http://gameworkslab.jp/2018/10/13/c%e8%a8%80%e8%aa%9e%e3%81%a7%e7%b0%a1%e5%8d%98%e3%81%aa%e3%82%ad%e3%83%bc%e5%85%a5%e5%8a%9b%e3%82%b7%e3%82%b9%e3%83%86%e3%83%a0%e3%82%ad%e3%83%bc%e3%83%90%e3%82%a4%e3%83%b3%e3%83%89%e4%bb%98%e3%81%8d/2/

/* --- ヘッダファイルのインクルード ---- */

#include <Windows.h>
#include <WinUser.h>

#include "Arithmetic/Arithmetic.h"
#include "Utility/Singleton.h"

#include "Cursor.h"
#include "Keylist.h"
#include "Keyboard.h"
#include "GamePad.h"

/* --- クラス宣言 ---- */

/// <summary>
/// 入力クラス
/// </summary>
class Input : public Singleton<Input>
{
public:

	Input();

	void Initialize(HWND hwnd);

	void Update(float elapsed_time);

public:

	float GetVerticalWheelState() const { return vertical_wheel_state; }

	// マウスホイール値を加算する
	void AddVerticalWheelState(float value) { /*if(keyboard.GetKeyState(VK::LCONTROL) & Keyboard::On)*/ vertical_wheel_state += value; }

	const GamePad&  GetGamePad()  const { return gamepad;  }
	const Keyboard& GetKeyboard() const { return keyboard; }
	const Cursor&   GetCursor()	  const { return cursor;   }

private:
	
	void UpdateWheel(float elapsed_time);

	HWND hwnd;

	Keyboard keyboard;
	GamePad gamepad;
	Cursor cursor;

	float vertical_wheel_state;
};
