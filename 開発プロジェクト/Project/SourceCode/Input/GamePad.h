#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <Windows.h> // XInput����ɃC���N���[�h���Ȃ��ƃG���[!!!
#include <Xinput.h>
#include <array>

/* --- �錾 ---- */

/// <summary>
/// XInput���g�p
/// </summary>
class GamePad
{
public:

	enum Button : uint16_t
	{
		DPAD_UP		   =  XINPUT_GAMEPAD_DPAD_UP		, // �\����
		DPAD_DOWN	   =  XINPUT_GAMEPAD_DPAD_DOWN		, // �\����
		DPAD_LEFT	   =  XINPUT_GAMEPAD_DPAD_LEFT		, // �\����
		DPAD_RIGHT	   =  XINPUT_GAMEPAD_DPAD_RIGHT	    , // �\���E
		DPAD_START	   =  XINPUT_GAMEPAD_START		    , // �^�񒆉E
		DPAD_BACK	   =  XINPUT_GAMEPAD_BACK			, // �^�񒆍�
		LEFT_THUMB     =  XINPUT_GAMEPAD_LEFT_THUMB		, // �X�e�B�b�N��������
		RIGHT_THUMB    =  XINPUT_GAMEPAD_RIGHT_THUMB	, // �X�e�B�b�N��������
		LEFT_SHOULDER  =  XINPUT_GAMEPAD_LEFT_SHOULDER  , // 
		RIGHT_SHOULDER =  XINPUT_GAMEPAD_RIGHT_SHOULDER , // 
		A			   =  XINPUT_GAMEPAD_A				, // 
		B			   =  XINPUT_GAMEPAD_B				, // 
		X			   =  XINPUT_GAMEPAD_X				, // 
		Y			   =  XINPUT_GAMEPAD_Y				, // 
	};

	enum Flag : uint8_t
	{
		OFF  = 0      ,	// ������Ă��Ȃ�
		ON   = 1 << 0 ,	// �����Ă���
		DOWN = 1 << 1 ,	// �������u��
		UP   = 1 << 2 ,	// �������u��
	};

	void Update();

	/// <summary>
	/// �����̃{�^���̌��݂ƑO�̃t���[���̏�Ԃ��r����B
	/// �����̃t���O���r�b�g�a�ō��킹�Ĕ���ł���B
	/// </summary>
	/// <param name="button">���͏�Ԃ�����{�^��</param>
	/// <param name="flag">�ǂ̓��͏�Ԃ̔��肩</param>
	/// <returns></returns>
	bool GetButton(Button button, uint8_t flag) const;

	DWORD GetButton() const
	{
		return input_state[0].Gamepad.wButtons;
	}

	/// <summary>
	/// 0���X�e�B�b�N�̒��S�Ƃ��āA(+-)deadzone�͈̔͂�������Short�^�̒l�͈̔͂�
	/// (-1 ~ 1)�ɐ��K������B
	/// </summary>
	/// <param name="deadzone">���͂̔�����s��Ȃ��͈�</param>
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
	/// X��Y�̃x�N�g���̒������P�𒴂���ꍇ�A�P�ɂȂ�悤�ɐ��K��
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
	/// 0���X�e�B�b�N�̒��S�Ƃ��āA(+-)deadzone�͈̔͂�������Short�^�̒l�͈̔͂�
	/// (-1 ~ 1)�ɐ��K������B
	/// </summary>
	float Normalize(int16_t thumb, int16_t deadzone) const;

	/// <summary>
	/// X��Y�̃x�N�g���̒������P�𒴂���ꍇ�A�P�ɂȂ�悤�ɐ��K��
	/// </summary>
	std::pair<float, float> Normalize(int16_t thumb_x, int16_t thumb_y, int16_t deadzone) const;

	/// <summary>
	/// �R���g���[���[���ڑ�����Ă��Ȃ��ꍇ��false��Ԃ�
	/// </summary>
	bool UpdateInputState();
	
	// [0] ���݂̃t���[���̓��͏�� [1] �O�̃t���[���̓��͏��
	std::array<XINPUT_STATE, 2> input_state;

	bool is_connecting_controller;
};
