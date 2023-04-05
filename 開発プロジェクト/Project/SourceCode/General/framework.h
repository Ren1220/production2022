#pragma once

#include <tchar.h>
#include <sstream>
#include <format>

#define USE_IMGUI
#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_ja_gryph_ranges.h"
#include "implot.h"
#endif // USE_IMGUI

#include "Include/misc.h"
#include "Input/Input.h"
#include "Graphics/General/Graphics.h"
#include "Scene/SceneManager.h"

#include "AtaiEngine.h"
#include "high_resolution_timer.h"

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

static bool reset_imgui{ false };

class Framework
{
public:

	Framework(HWND hwnd) : hwnd(hwnd)
	{
		
	}

	~Framework();

private:

	const HWND hwnd;

public:

	int Run()
	{
		MSG msg = {};

		if (!Initialize()) return 0;
#ifdef USE_IMGUI
		// ImGui開始処理
		ImGui::CreateContext();
		// 2022.10.20 ImPlot追加
		ImPlot::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\YuGothB.ttc", 14.0f, nullptr, glyphRangesJapanese);
		ImFontConfig guiFontConfig;
		guiFontConfig.MergeMode = true;
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(MySystem.GetDevice(), MySystem.GetDeviceContext());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				CalculateFrameStats();
				Update(tictoc.time_interval());
				Render(tictoc.time_interval());

				if (reset_imgui)
				{
#ifdef USE_IMGUI
					// ウィンドウを変更したらImGuiの初期化を行う。（これしてもカーソルのずれが治らないので、別の原因がありそう）

					// ImGui終了処理
					ImGui_ImplDX11_Shutdown();
					ImGui_ImplWin32_Shutdown();
					ImGui::DestroyContext();
					ImPlot::DestroyContext();

					// ImGui開始処理
					ImGui::CreateContext();
					ImPlot::CreateContext();
					ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\YuGothB.ttc", 14.0f, nullptr, glyphRangesJapanese);
					ImFontConfig guiFontConfig;
					guiFontConfig.MergeMode = true;
					ImGui_ImplWin32_Init(hwnd);
					ImGui_ImplDX11_Init(MySystem.GetDevice(), MySystem.GetDeviceContext());
					ImGui::StyleColorsDark();
#endif
					reset_imgui = false;
				}
			}
		}

#ifdef USE_IMGUI
 		// ImGui終了処理
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		// 2022.10.20 ImPlot追加
		ImPlot::DestroyContext();

#endif

		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {};
#endif // USE_IMGUI

		static int32_t wheel_fraction{ 0 };
		int32_t delta;
		float notch;

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			tictoc.start();
			break;

		case WM_SIZE:
		{
			RECT client_rect{};
			GetClientRect(hwnd, &client_rect);
			//MySystem.Resize(static_cast<UINT>(client_rect.right - client_rect.left), static_cast<UINT>(client_rect.bottom - client_rect.top));

			// ウィンドウを変更したらImGuiの初期化を行う。（これしてもカーソルのずれが治らないので、別の原因がありそう）
			reset_imgui = true;

			break;
		}
		case 0x20A: // マウスホイール検知
		{


			delta = GET_WHEEL_DELTA_WPARAM(wparam);

			delta += wheel_fraction;

			// ノッチ数を求める
			notch = static_cast<float>(delta / WHEEL_DELTA);

			// 端数を保存する
			wheel_fraction = delta % WHEEL_DELTA;

			Input::GetInstance().AddVerticalWheelState(notch);



			//if (notch > 0)
			//{
			//	// 上に回転した
			//	Input.AddVerticalWheelState(notch);
			//}
			//else if (notch < 0)
			//{
			//	// 下に回転した
			//	Input.AddVerticalWheelState(notch);
			//}
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}


private:

	bool Initialize();
	void Update(float elapsed_time);
	void Render(float elapsed_time);

private:

	high_resolution_timer tictoc;
	uint32_t frames_per_second{ 0 };
	float count_by_seconds{ 0.0f };
	void CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		if (++frames_per_second, (tictoc.time_stamp() - count_by_seconds) >= 1.0f)
		{
			float fps = static_cast<float>(frames_per_second);
			std::wostringstream outs;
			outs.precision(6);
			outs << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
#ifdef DEBUG
			SetWindowTextW(hwnd, outs.str().c_str());
#else
			SetWindowTextW(hwnd, L"Shaze");
#endif // DEBUG

			frames_per_second = 0;
			count_by_seconds += 1.0f;
		}
	}
};