#pragma once

/* --- ヘッダファイルのインクルード ---- */

// マルチスレッド中に同じ変数にアクセスさせないための処理を行う
#include <mutex> 

#include <stdint.h>
#include <filesystem>

#include "Utility/Singleton.h"
#include "Graphics/General/DirectXUser.h"


/* --- クラスの定義 ---- */

/// <summary>
/// DirectX全般
/// </summary>
class Engine : public Singleton<Engine>
{
public:

	Engine();
	~Engine() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="set_hwnd"></param>
	/// <param name="is_fullscreen">スクリーンモード</param>
	void Initialize(const HWND set_hwnd, bool is_fullscreen);
	/// <summary>
	/// スワップチェーンのバッファをデバイスコンテキストに設定する。
	/// </summary>
	void ActivateSwapChainBuffer();
	/// <summary>
	/// ウィンドウに描画する
	/// </summary>
	void Present();
	/// <summary>
	/// スクリーンサイズの変更
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void Resize(UINT width, UINT height);
	/// <summary>
	/// スクリーンモードの変更
	/// </summary>
	void ChangeScreenMode(bool is_fullscreen);

	/// <summary>
	/// カレントディレクトリが変化した際に開始時のカレントディレクトリに戻すために使用する。　
	/// </summary>
	void RestoreCurrentDirctory() const;

	ID3D11Device* GetDevice() { return directx.GetDevice(); }
	ID3D11DeviceContext* GetDeviceContext() { return directx.GetDeviceContext(); }
	ID2D1DeviceContext* GetDeviceContext2D() { return directx.GetDeviceContext2D(); }
	ID2D1Factory1* GetFactory2D() { return directx.GetFactory2D(); }
	std::mutex& GetMutex() { return mutex; }

	bool IsFullscreen() const { return is_fullscreen; }

	template <class T>
	std::pair<T, T> GetScreenSize() const
	{
		return { static_cast<T>(framebuffer_dimensions.cx), static_cast<T>(framebuffer_dimensions.cy) };
	}

	/// <summary>
	/// 描画時の設定
	/// </summary>
	/// <param name="flag">DXGI_PRESENT_ALLOW_TEARINGでfpsの上限を解除する</param>
	void SetSwapChainPresentFlag(UINT flag);

private:

	/// <summary>
	/// フルスクリーン、ウィンドウの切り替え設定
	/// </summary>
	void SetupFullScreen();

	void SetupWindow();

private:

	HWND hwnd = NULL;
	std::mutex mutex;
	bool is_fullscreen;
	RECT window_rect;
	DWORD window_style;
	SIZE framebuffer_dimensions;

	DirectXUser directx;
	UINT swap_chain_present_flag; // DXGI_PRESENT_ALLOW_TEARINGでfpsの上限を解除する

	std::string project_directory; // ダイアログボックスを使用することでカレントディレクトリが変化するため、開始時のディレクトリパスを確保しておく。

};

/* --- マクロの定義 ---- */

#define MySystem (Engine::GetInstance())