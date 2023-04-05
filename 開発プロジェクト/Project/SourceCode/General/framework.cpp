#include "framework.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Audio/AudioManager.h"
#include "Debug/DebugPrimitive.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Debug/DebugPrimitive2D.h"

#include "Scene/SceneLoading.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"

/* --- 定義 ---- */

Framework::~Framework()
{
    SceneManager::GetInstance().EndProcess();
    Graphics::GetInstance().EndProcess(MySystem.GetDeviceContext());

    AudioManager::GetInstance().EndProcess();
}

bool Framework::Initialize()
{
    // キー入力の初期化処理
	Input::GetInstance().Initialize(hwnd);

    // DirectXの初期化
    MySystem.Initialize(hwnd, false);
    MySystem.SetSwapChainPresentFlag(0);
    auto [width, height] { MySystem.GetScreenSize<UINT>() };

    AudioManager::GetInstance().Initialize();

    ID3D11Device* device{ MySystem.GetDevice() };
    Graphics::GetInstance().Initialize(device, width, height);

    // デバックプリミティブ描画クラスの初期化
    DebugPrimitive::GetInstance().Initialize(device);
    DebugPrimitive2D::GetInstance().Initialize(device);

    // 新しいシーンの設定
    SceneManager::GetInstance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<InitialScene>()));

    return true;
}

void Framework::Update(float elapsed_time)
{
    // キー入力の更新処理
    Input::GetInstance().Update(elapsed_time);
    
    // オーディオの更新
    AudioManager::GetInstance().Update();

    // 現在のシーンの更新処理
    SceneManager::GetInstance().Update(elapsed_time);

#ifdef USE_IMGUI
    // ImGui更新処理
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif
}

void Framework::Render(float elapsed_time)
{
    // 別スレッド中にデバイスコンテキストが使われていた場合に
    // 同時アクセスしないように排他制御する
    std::lock_guard<std::mutex> lock(MySystem.GetMutex());

    ID3D11DeviceContext* dc{ MySystem.GetDeviceContext() };

    // 現在のシーンの描画処理
    SceneManager::GetInstance().Render(dc);

    // ImGui描画処理
#ifdef USE_IMGUI
    SceneManager::GetInstance().ImGuiRender();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // USE_IMGUI

    // 画面に描画する ここの第二引数で可変フレームレートになる
    MySystem.Present();
}
