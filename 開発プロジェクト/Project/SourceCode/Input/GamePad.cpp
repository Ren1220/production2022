#include "GamePad.h"

/* --- 定義 ---- */

void GamePad::Update()
{
	// 入力状態
	is_connecting_controller = UpdateInputState();
}

bool GamePad::GetButton(Button button, uint8_t flag) const
{
	bool current, previous;
	current  = input_state[0].Gamepad.wButtons & button;
	previous = input_state[1].Gamepad.wButtons & button;

	if      (flag & Flag::OFF) { return !current && !previous; }
	else if (flag & Flag::ON)  { return  current &&  previous; }
	else if (flag & Flag::DOWN){ return  current && !previous; }
	else if (flag & Flag::UP)  { return !current &&  previous; }

	return false;
}

bool GamePad::GetTriggerL(uint8_t flag) const
{
	bool current, previous;

	current  = input_state[0].Gamepad.bLeftTrigger > 0;
	previous = input_state[1].Gamepad.bLeftTrigger > 0;

	if (flag & Flag::OFF) { return !current && !previous; }
	else if (flag & Flag::ON) { return  current && previous; }
	else if (flag & Flag::DOWN) { return  current && !previous; }
	else if (flag & Flag::UP) { return !current && previous; }

	return false;
}

bool GamePad::GetTriggerR(uint8_t flag) const
{
	bool current, previous;

	current  = input_state[0].Gamepad.bRightTrigger > 0;
	previous = input_state[1].Gamepad.bRightTrigger > 0;

	if (flag & Flag::OFF) { return !current && !previous; }
	else if (flag & Flag::ON) { return  current && previous; }
	else if (flag & Flag::DOWN) { return  current && !previous; }
	else if (flag & Flag::UP) { return !current && previous; }

	return false;
}

float GamePad::Normalize(int16_t thumb, int16_t deadzone) const
{
	//   0.0f <--------------> 1.0f
	// deadzone              INT16_MAX
	float v{ (std::max)(0.0f, static_cast<float>(abs(thumb) - deadzone)) / static_cast<float>(INT16_MAX - deadzone) };
	// 入力値が負の場合はマイナス
	return thumb > -1 ? v : -v;
}

std::pair<float, float> GamePad::Normalize(
	int16_t thumb_x,
	int16_t thumb_y, 
	int16_t deadzone) const
{
	float x{ Normalize(thumb_x, deadzone) };
	float y{ Normalize(thumb_y, deadzone) };
	
	float l{ sqrtf(x * x + y * y) };

	// ゼロ除算を回避
	if (l < FLT_EPSILON)
	{
		return { 0.0f,0.0f };
	}

	//float dx{ abs(x) > FLT_EPSILON ? x / l : 0.0f };
	//float dy{ abs(y) > FLT_EPSILON ? y / l : 0.0f };
	if (l > 1.0f)
	{
		return { x / l, y / l };
	}

	return { x ,y };
}

bool GamePad::UpdateInputState()
{
	// 過去の状態を保存
	memcpy_s(&input_state[1], sizeof(input_state[1]), &input_state[0], sizeof(input_state[0]));

	// 現在の状態を取得
	if (XInputGetState(0, &input_state[0]) != ERROR_SUCCESS)
	{
		// コントローラーが接続されていない
		return false;
	}

	return true;
}
