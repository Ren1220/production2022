#pragma once

/* --- ヘッダファイルのインクルード ---- */

/* --- 定義 ---- */

/// <summary>
/// 入力キーリスト
/// </summary>
enum class VK : unsigned char
{
	LBUTTON = 0x01,
	RBUTTON = 0x02,
	MBUTTON = 0x04,
	MBUTTON1 = 0x05,
	MBUTTON2 = 0x06,
	BACK = 0x08,
	TAB = 0x09,
	RETURN = 0x0D,
	SHIFT = 0x10,
	LSHIFT = 0xA0,
	RSHIFT = 0xA1,
	CONTROL = 0x11,
	LCONTROL = 0xA2,
	RCONTROL = 0xA3,
	ALT = 0x12,	//MENU
	LALT = 0xA4,	//MENU
	RALT = 0xA5,	//MENU
	ESCAPE = 0x1B,
	SPACE = 0x20,
	_DELETE = 0x2e,

	LEFT = VK_LEFT,
	UP = VK_UP,
	RIGHT = VK_RIGHT,
	DOWN = VK_DOWN,

	NUM_0 = 0x30,
	NUM_1 = 0x31,
	NUM_2 = 0x32,
	NUM_3 = 0x33,
	NUM_4 = 0x34,
	NUM_5 = 0x35,
	NUM_6 = 0x36,
	NUM_7 = 0x37,
	NUM_8 = 0x38,
	NUM_9 = 0x39,

	OFFPAD_0 = 0x2d, //NUMLOCK OFF 
	OFFPAD_1 = 0x23, //NUMLOCK OFF 
	OFFPAD_2 = 0x28, //NUMLOCK OFF 
	OFFPAD_3 = 0x22, //NUMLOCK OFF //pgdown
	OFFPAD_4 = 0x25, //NUMLOCK OFF 
	OFFPAD_5 = 0x0c, //NUMLOCK OFF 
	OFFPAD_6 = 0x27, //NUMLOCK OFF 
	OFFPAD_7 = 0x24, //NUMLOCK OFF 
	OFFPAD_8 = 0x26, //NUMLOCK OFF 
	OFFPAD_9 = 0x21, //NUMLOCK OFF //pgup

	ONPAD_0 = 0x60,	 //NUMLOCK ON
	ONPAD_1 = 0x61,	 //NUMLOCK ON
	ONPAD_2 = 0x62,	 //NUMLOCK ON
	ONPAD_3 = 0x63,	 //NUMLOCK ON
	ONPAD_4 = 0x64,	 //NUMLOCK ON
	ONPAD_5 = 0x65,	 //NUMLOCK ON
	ONPAD_6 = 0x66,	 //NUMLOCK ON
	ONPAD_7 = 0x67,	 //NUMLOCK ON
	ONPAD_8 = 0x68,	 //NUMLOCK ON
	ONPAD_9 = 0x69,	 //NUMLOCK ON

	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A,

	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,
	F13 = 0x7C,
	F14 = 0x7D,
	F15 = 0x7E,
	F16 = 0x7F,
	F17 = 0x80,
	F18 = 0x81,
	F19 = 0x82,
	F20 = 0x83,
	F21 = 0x84,
	F22 = 0x85,
	F23 = 0x86,
	F24 = 0x87,

	COLON = 0xBA, // KEY is [ : ] and [ * ] 
	PLUS = 0xBB, // KEY is [ ; ] and [ + ] 
	COMMA = 0xBC,	// KEY is [ , ] and [ < ] 
	MINUS = 0xBD,	// KEY is [ - ] and [ = ] 
	PERIOD = 0xBE,	// KEY is [ . ] and [ > ] 
	QUESTION = 0xBF,	// KEY is [ / ] and [ ? ] 
	AT = 0xC0,	// KEY is [ @ ] and [ ` ] 
	SQUARE_BRACKET_L = 0xDB,	// KEY is [ [ ] and [ { ] 
	VERTICAL_VAR = 0xDC,	// KEY is [ \ ] and [ | ] 
	SQUARE_BRACKET_R = 0xDD,	// KEY is [ ] ] and [ } ] 
	CARET = 0xDE,	// KEY is [ ^ ] and [ ~ ] 
};
