#include "Cursor.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <cassert>

/* --- 定義 ---- */

void Cursor::Update(HWND hwnd)
{
	ObtainPosition(hwnd);
}

void Cursor::ObtainPosition(HWND hwnd)
{
	POINT point{};

	// スクリーン内における座標
	GetCursorPos(&point);
	position[1] = position[0]; // 前のフレームの座標を渡す。
	position[0] = point;

	// ウィンドウ内における座標
	BOOL succeded{ ScreenToClient(hwnd, &point) };
	_ASSERT_EXPR(succeded, GetLastError());
	position[3] = position[2];
	position[2] = point;
}
