#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <unordered_map>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "Arithmetic/Arithmetic.h"

#include "EffekseerEffect.h"

/* --- 定義 ---- */

class EffekseerManager
{
public:


	EffekseerManager() {}	
	~EffekseerManager() {}

	/// <summary>
	/// 初期化処理
	/// 内部でデバイスコンテキストを使用しているため、
	/// ミューテックスによってマルチスレッドの同時アクセスを防いでいる。
	/// </summary>
	void Initialize(ID3D11Device* device);
	/// <summary>
	/// 終了処理
	/// </summary>
	void EndProcess();

	void Update(float elapsed_time);
	void Render(const Matrix& view, const Matrix& projection);
	void ImGuiRender();

	/// <summary>
	/// 全てのエフェクトを停止させる
	/// </summary>
	void StopAllEffects();
	/// <summary>
	/// ハンドルのエフェクトを停止する
	/// </summary>
	void StopEffect(Effekseer::Handle handle);
	/// <summary>
	/// ハンドルのエフェクトを描画するか
	/// </summary>
	void SetShownEffect(Effekseer::Handle handle, bool is_shown);

	Effekseer::Manager* GetEffekseerManager() { return effekseer_manager; }

	/// <summary>
	/// 引数のファイルパスのエフェクトを発生させる
	/// </summary>
	/// <param name="filepath">"Data/Effect/"に続くファイルパス</param>
	/// <returns>発生させたエフェクトを操作するためのハンドルID</returns>
	Effekseer::Handle PlayEffect(const char* filepath, const Vector3& position);

	/// <summary>
	/// エフェクトを生成する、予め生成することでゲーム中の読み込みの発生を避ける
	/// </summary>
	/// <param name="filepath">"Data/Effect/"に続くファイルパス</param>
	/// <returns>発生させたエフェクトを操作するためのハンドルID</returns>
	void LoadEffect(const char* filepath);

	/// <summary>
	/// 引数のディレクトリ以下のefkファイルをすべて読み込む
	/// </summary>
	/// <param name="directory"></param>
	/// <returns></returns>
	void LoadEffectRecursive(const wchar_t* directory_path);

private:



	Effekseer::Manager* effekseer_manager{ nullptr };
	EffekseerRenderer::Renderer* effekseer_renderer{ nullptr };

	std::unordered_map<std::string, EffekseerEffect> effects;

};