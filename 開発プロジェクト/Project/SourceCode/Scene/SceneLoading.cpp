#include "SceneLoading.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <thread>

#include "Include/misc.h"

#include "General/AtaiEngine.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Utility/ChronoTimer.h"
#include "SceneManager.h"

/* --- 定義 ---- */

SceneLoading::SceneLoading(std::unique_ptr<Scene>&& next_scene)
	: next_scene(std::move(next_scene))
{
	// アニメーションの存在するフォルダのパスを入力
	loading = std::make_unique<TextureAnimation>("./Data/Texture/General/Loadings");

	// アニメーションフレームを設定
	loading->SetAnimationFrame(loading->CalculateAnimationFramePerTime(1.0f));
}

void SceneLoading::Initialize()
{
	/*
		スレッドを立ち上げた後thread関数が破棄されるまでに、
		（今回はInitialize()関数が終了するとき）
		Join()関数でスレッドの終了を待つか、
		Detouch()でスレッドの管理を放棄しなければエラーになる
	*/

	// ロード中はfps制限なし
	MySystem.SetSwapChainPresentFlag(DXGI_PRESENT_ALLOW_TEARING);

	// スレッド開始
	std::thread thread(LoadingThread, this);

	// スレッドの管理を放棄
	thread.detach();
}

void SceneLoading::EndProcess()
{
	ResourceFileLoader::ReleaseAllTexture();
}

void SceneLoading::Update(float elapsed_time)
{
	loading->Update(elapsed_time);

	// 次のシーンの準備が完了したらシーンを切り替える
	if (next_scene->IsReady())
	{
		SceneManager::GetInstance().ChangeScene(std::move(next_scene));
		MySystem.SetSwapChainPresentFlag(0);
		return;
	}
}

void SceneLoading::Render(ID3D11DeviceContext* dc)
{
	auto [width, height] { MySystem.GetScreenSize<float>() };

	MySystem.ActivateSwapChainBuffer();

	loading->Render(dc, width - loading->GetTextureWidth() + 120, height - loading->GetTextureHeight() + 50, loading->GetTextureWidth(), loading->GetTextureHeight());
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	HRESULT hr{ S_OK };
	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	hr = CoInitialize(nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ChronoTimer time;
	time.Start();
	// 次のシーンの初期化を行う
	scene->next_scene->Initialize();
	time.Stop(true, L"シーン初期化時間");

	// スレッドが終わる前にCOM関連の終了処理
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->next_scene->SetReady();
}
