#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <Windows.h>
#include <WinUser.h>
#include <array>

/* --- 宣言 ---- */

class Cursor
{
public:

	Cursor() = default;
	~Cursor() = default;

	void Update(HWND hwnd);

	/// <summary>
	// スクリーン内における座標
	/// </summary>
	const POINT& GetCurrentAbsPosition() const { return position[0]; }
	const POINT& GetPreviousAbsPosition() const { return position[1]; }

	/// <summary>
	// ウィンドウ内における座標
	/// </summary>
	const POINT& GetCurrentPosition() const { return position[2]; }
	const POINT& GetPreviousPosition() const { return position[3]; }

private:

	/// <summary>
	/// カーソルの座標を入手する
	/// </summary>
	void ObtainPosition(HWND hwnd);
	
	// [0][1] abs, [2][3] relative
	std::array<POINT, 4> position;
};
