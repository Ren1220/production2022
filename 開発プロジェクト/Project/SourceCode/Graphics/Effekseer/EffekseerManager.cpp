#include "EffekseerManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"

#include "General/AtaiEngine.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"

/* --- 定義 ---- */

void EffekseerManager::Initialize(ID3D11Device* device)
{
	// デバイスコンテキストの同時アクセスを防ぐ
	std::lock_guard<std::mutex> lock(MySystem.GetMutex());

	// Effekseerレンダラー生成
	effekseer_renderer = EffekseerRendererDX11::Renderer::Create(device, MySystem.GetDeviceContext(), 2048);

	// Effekseerマネージャー生成
	effekseer_manager = Effekseer::Manager::Create(2048);

	// Effekseerレンダラの各種設定(カスタマイズする場合は変更する)
	effekseer_manager->SetSpriteRenderer(effekseer_renderer->CreateSpriteRenderer());
	effekseer_manager->SetRibbonRenderer(effekseer_renderer->CreateRibbonRenderer());
	effekseer_manager->SetRingRenderer(effekseer_renderer->CreateRingRenderer());
	effekseer_manager->SetTrackRenderer(effekseer_renderer->CreateTrackRenderer());
	effekseer_manager->SetModelRenderer(effekseer_renderer->CreateModelRenderer());
	// Effekseer内でのローダーの設定(同上)
	effekseer_manager->SetTextureLoader(effekseer_renderer->CreateTextureLoader());
	effekseer_manager->SetModelLoader(effekseer_renderer->CreateModelLoader());
	effekseer_manager->SetMaterialLoader(effekseer_renderer->CreateMaterialLoader());

	// Effekseerを左手座標系で計算(DirectXは左手座標系)
	effekseer_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffekseerManager::EndProcess()
{
	effects.clear();

	effekseer_manager->Destroy();
	effekseer_renderer->Destroy();
}

void EffekseerManager::Update(float elapsed_time)
{
	//エフェクト更新処理(引数にはフレームの経過時間を渡す)
	effekseer_manager->Update(elapsed_time * 60.0f);
}

void EffekseerManager::Render(const Matrix& view, const Matrix& projection)
{
	//ビュー&プロジェクション行列をEffekseerレンダラに設定
	effekseer_renderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseer_renderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//Effekseer描画開始
	effekseer_renderer->BeginRendering();

	//Effekseer描画実行
	//マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
	//Draw()関数を実行する順序で制御できる
	effekseer_manager->Draw();

	//Effekseer描画終了
	effekseer_renderer->EndRendering();
}

void EffekseerManager::ImGuiRender()
{
	auto func = [](void* data, int n, const char** out_text) ->bool
	{
		auto effects{ reinterpret_cast<std::unordered_map<std::string, EffekseerEffect>*>(data) };

		if (effects->size() <= static_cast<size_t>(n)) return false;
		auto itr{ std::next(effects->begin(), n) };
		*out_text = itr->first.c_str();
		return true;
	};

	static int itrpos{ 0 };
	ImGui::ListBox("EffectList", &itrpos, func, &effects, static_cast<int>(effects.size()));
	
	if (ImGui::Button("Play"))
	{
		if (auto camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera())
		{
			auto itr{ std::next(effects.begin(), itrpos) };
			itr->second.Play(camera->GetFocus());
		}
	}

}

void EffekseerManager::StopAllEffects()
{
	// エフェクトの動作を停止させる
	effekseer_manager->StopAllEffects();
}

void EffekseerManager::SetShownEffect(
	Effekseer::Handle handle, 
	bool is_shown)
{
	effekseer_manager->SetShown(handle, is_shown);
}

Effekseer::Handle EffekseerManager::PlayEffect(const char* filepath, const Vector3& position)
{
	// エフェクトのディレクトリ
	std::string directory { "Data/Effect/" };
	directory += filepath;

	auto found_itr{ effects.find(directory) };
	// 既に作成したエフェクト
	if (found_itr != effects.end())
	{
		return found_itr->second.Play(position);
	}
	// 新しいエフェクト
	else
	{
		auto& new_effect{ effects[directory] };


		new_effect.Create(effekseer_manager, directory.c_str());
		return new_effect.Play(position);
	}
}

void EffekseerManager::LoadEffect(const char* filepath)
{
	auto found_itr{ effects.find(std::filesystem::absolute(filepath).string()) };

	// 新しいエフェクト
	if (found_itr == effects.end())
	{
		auto& new_effect{ effects[filepath] };

		new_effect.Create(effekseer_manager, filepath);
		return;
	}
}

void replaceOtherStr(std::string& replacedStr, std::string from, std::string to) {
	const size_t pos = replacedStr.find(from);
	const int len = static_cast<int>(from.length());

	if (pos == std::string::npos || from.empty()) {
		return;
	}

	replacedStr.replace(pos, len, to);
	replaceOtherStr(replacedStr, from, to);
	return;
}

void EffekseerManager::LoadEffectRecursive(const wchar_t* directory_path)
{
	namespace fs = std::filesystem;

	_ASSERT_EXPR(fs::is_directory(directory_path), L"引数のディレクトリが存在しません");

	const auto h_extension{ std::hash<fs::path>()(".efk") };

	for (const auto& e : fs::recursive_directory_iterator(directory_path))
	{
		if (h_extension == std::hash<fs::path>()(e.path().extension()))
		{
			std::string path{ e.path().string() };
			replaceOtherStr(path, "\\", "/");
			LoadEffect(path.c_str());
		}
	}
}

void EffekseerManager::StopEffect(Effekseer::Handle handle)
{
	effekseer_manager->StopEffect(handle);
}