#include "AtaiEngine.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <string>
#include <vector>

#include "Include/misc.h"

/* --- 関数の実装 ---- */

Engine::Engine()
	: is_fullscreen(false), window_rect(), window_style(), framebuffer_dimensions(), 
	  swap_chain_present_flag(DXGI_PRESENT_ALLOW_TEARING)
{
	// ダイアログボックスを使用することでカレントディレクトリが変化するため、開始時のディレクトリパスを確保しておく。
	std::string dir;
	dir.resize(260);
	GetCurrentDirectoryA(static_cast<DWORD>(dir.size()), dir.data());
	dir.erase(dir.begin() + dir.find_first_of((char)0), dir.end());
	project_directory = dir;
}

void Engine::Initialize(const HWND hwnd, bool is_fullscreen)
{
	HRESULT hr{ S_OK };

	this->hwnd = hwnd;

	this->is_fullscreen = is_fullscreen;

	if (is_fullscreen)
	{
		SetupFullScreen();
	}

	// フレームバッファのサイズを計算する
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	framebuffer_dimensions.cx = client_rect.right - client_rect.left;
	framebuffer_dimensions.cy = client_rect.bottom - client_rect.top;

	// 現在のウィンドウの状態（初期の状態を）保存する
	window_style = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));

	// DirectX初期化
	directx.Initialize(hwnd, framebuffer_dimensions.cx, framebuffer_dimensions.cy);

	swap_chain_present_flag = DXGI_PRESENT_ALLOW_TEARING;
}

void Engine::ActivateSwapChainBuffer()
{
	auto [wf, hf] { GetScreenSize<float>() };
	directx.ActivateSwapChainBuffer(wf, hf);
}

void Engine::Present()
{
	directx.Present(0, swap_chain_present_flag);
}

void Engine::Resize(UINT width, UINT height)
{
	/* --- 処理 ---- */

	HRESULT hr{ S_OK };

	auto [uw, uh] { GetScreenSize<UINT>() };

	// ウィンドウのサイズが異なる場合
	if (width != uw || height != uh )
	{
		framebuffer_dimensions.cx = static_cast<LONG>(width);
		framebuffer_dimensions.cy = static_cast<LONG>(height);

		// DirectX系の再設定
		directx.Resize(
			static_cast<UINT>(framebuffer_dimensions.cx),
			static_cast<UINT>(framebuffer_dimensions.cy));

		// スクリーンの再設定を行う
		SetupWindow();
	}
}

void Engine::ChangeScreenMode(bool is_fullscreen)
{
	if (this->is_fullscreen != is_fullscreen)
	{
		this->is_fullscreen = is_fullscreen;

		// スクリーンモードが変更された為、
		// スクリーンの再設定を行う
		SetupFullScreen();
	}
}

void Engine::RestoreCurrentDirctory() const
{
	SetCurrentDirectoryA(project_directory.c_str());

	char a[100]{ };
	GetCurrentDirectoryA(100, a);
	OutputDebugStringA(a);
}

void Engine::SetSwapChainPresentFlag(UINT flag)
{
	swap_chain_present_flag = flag;
}

void Engine::SetupFullScreen()
{
	/* --- 処理 ---- */

	if (is_fullscreen)
	{
		// フルスクリーンモードの時の設定

		// ウィンドウのサイズの取得
		GetWindowRect(hwnd, &window_rect);

		// ウィンドウの状態を変更する
		SetWindowLongPtrA(
			hwnd,			// ウィンドウのハンドルの 
			GWL_STYLE,		// ウィンドウの状態を新しく設定するフラグ
			//https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
			WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
		);

		// フルスクリーンモードのウィンドウのサイズ
		RECT fullscreen_window_rect;

		DEVMODE devmode{};
		devmode.dmSize = sizeof(DEVMODE);
		// https://www.tokovalue.jp/function/EnumDisplaySettings.htm
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

		// フルスクリーンモードのウィンドウのサイズを設定する
		fullscreen_window_rect =
		{
			devmode.dmPosition.x,
			devmode.dmPosition.y,
			devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
			devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
		};

		// ウィンドウのサイズを設定する https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
		SetWindowPos(
			hwnd,
#ifdef _DEBUG
			NULL,
#else
			HWND_TOPMOST,
#endif // _DEBUG
			fullscreen_window_rect.left,
			fullscreen_window_rect.top,
			fullscreen_window_rect.right,
			fullscreen_window_rect.bottom,
			SWP_FRAMECHANGED | // SetWindowLong関数で設定した新しいウィンドウスタイルを適用する
			SWP_NOACTIVATE	   // ウィンドウをアクティブ状態にしない
		);

		// ウィンドウを表示する https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
		ShowWindow(
			hwnd,
			SW_MAXIMIZE // ウィンドウを最大化した状態で表示する
		);
	}
	else
	{
		SetWindowLongPtrA(hwnd, GWL_STYLE, window_style);
		SetWindowPos(
			hwnd,
			HWND_NOTOPMOST,
			window_rect.left,
			window_rect.top,
			window_rect.right - window_rect.left,
			window_rect.bottom - window_rect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd, SW_NORMAL);
	}
}

void Engine::SetupWindow()
{
	// ウィンドウのサイズの取得
	GetWindowRect(hwnd, &window_rect);

	SetWindowLongPtrA(hwnd, GWL_STYLE, window_style);
	SetWindowPos(
		hwnd,
		HWND_NOTOPMOST,
		window_rect.left,
		window_rect.top,
		window_rect.left + framebuffer_dimensions.cx,
		window_rect.top + framebuffer_dimensions.cy,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);

	ShowWindow(hwnd, SW_NORMAL);
}
