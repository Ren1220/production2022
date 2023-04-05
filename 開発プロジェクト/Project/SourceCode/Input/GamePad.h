#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <Windows.h> // XInputより先にインクルードしないとエラー!!!
#include <Xinput.h>
#include <array>

/* --- 宣言 ---- */

/// <summary>
/// XInputを使用
/// </summary>
class GamePad
{
public:

	enum Button : uint16_t
	{
		DPAD_UP		   =  XINPUT_GAMEPAD_DPAD_UP		, // 十字上
		DPAD_DOWN	   =  XINPUT_GAMEPAD_DPAD_DOWN		, // 十字下
		DPAD_LEFT	   =  XINPUT_GAMEPAD_DPAD_LEFT		, // 十字左
		DPAD_RIGHT	   =  XINPUT_GAMEPAD_DPAD_RIGHT	    , // 十字右
		DPAD_START	   =  XINPUT_GAMEPAD_START		    , // 真ん中右
		DPAD_BACK	   =  XINPUT_GAMEPAD_BACK			, // 真ん中左
		LEFT_THUMB     =  XINPUT_GAMEPAD_LEFT_THUMB		, // スティック押し込み
		RIGHT_THUMB    =  XINPUT_GAMEPAD_RIGHT_THUMB	, // スティック押し込み
		LEFT_SHOULDER  =  XINPUT_GAMEPAD_LEFT_SHOULDER  , // 
		RIGHT_SHOULDER =  XINPUT_GAMEPAD_RIGHT_SHOULDER , // 
		A			   =  XINPUT_GAMEPAD_A				, // 
		B			   =  XINPUT_GAMEPAD_B				, // 
		X			   =  XINPUT_GAMEPAD_X				, // 
		Y			   =  XINPUT_GAMEPAD_Y				, // 
	};

	enum Flag : uint8_t
	{
		OFF  = 0      ,	// 押されていない
		ON   = 1 << 0 ,	// 押している
		DOWN = 1 << 1 ,	// 押した瞬間
		UP   = 1 << 2 ,	// 離した瞬間
	};

	void Update();

	/// <summary>
	/// 引数のボタンの現在と前のフレームの状態を比較する。
	/// 複数のフラグをビット和で合わせて判定できる。
	/// </summary>
	/// <param name="button">入力状態を見るボタン</param>
	/// <param name="flag">どの入力状態の判定か</param>
	/// <returns></returns>
	bool GetButton(Button button, uint8_t flag) const;

	DWORD GetButton() const
	{
		return input_state[0].Gamepad.wButtons;
	}

	/// <summary>
	/// 0をスティックの中心として、(+-)deadzoneの範囲を除いたShort型の値の範囲を
	/// (-1 ~ 1)に正規化する。
	/// </summary>
	/// <param name="deadzone">入力の判定を行わない範囲</param>
	float GetStickLX(int16_t deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbLX, deadzone);
	}
	float GetStickLY(int16_t deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbLY, deadzone);
	}
	float GetStickRX(int16_t deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbRX, deadzone);
	}
	float GetStickRY(int16_t deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbRY, deadzone);
	}

	bool GetTriggerL(uint8_t flag) const;

	bool GetTriggerR(uint8_t flag) const;
	
	/// <summary>
	/// XとYのベクトルの長さが１を超える場合、１になるように正規化
	/// </summary>
	std::pair<float, float> GetStickL(int16_t deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbLX, input_state[0].Gamepad.sThumbLY, deadzone);
	}
	std::pair<float, float> GetStickR(int16_t deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) const
	{
		return Normalize(input_state[0].Gamepad.sThumbRX, input_state[0].Gamepad.sThumbRY, deadzone);
	}

	bool IsConnectingController() const
	{
		return is_connecting_controller;
	}

private:

	/// <summary>
	/// 0をスティックの中心として、(+-)deadzoneの範囲を除いたShort型の値の範囲を
	/// (-1 ~ 1)に正規化する。
	/// </summary>
	float Normalize(int16_t thumb, int16_t deadzone) const;

	/// <summary>
	/// XとYのベクトルの長さが１を超える場合、１になるように正規化
	/// </summary>
	std::pair<float, float> Normalize(int16_t thumb_x, int16_t thumb_y, int16_t deadzone) const;

	/// <summary>
	/// コントローラーが接続されていない場合はfalseを返す
	/// </summary>
	bool UpdateInputState();
	
	// [0] 現在のフレームの入力状態 [1] 前のフレームの入力状態
	std::array<XINPUT_STATE, 2> input_state;

	bool is_connecting_controller;
};
