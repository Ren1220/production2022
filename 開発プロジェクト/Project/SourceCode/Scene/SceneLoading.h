#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "scene.h"
#include "Graphics/Resource/Texture/TextureAnimation.h"

/* --- 宣言 ---- */

class SceneLoading : public Scene
{
public:

	SceneLoading(std::unique_ptr<Scene>&& next_scene);
	~SceneLoading() = default;

	
public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;

private:

	/// <summary>
	/// 別スレッドで行う処理、
	/// Comオブジェクト、次のシーンの初期化を行う。
	/// 完了すると、sceneのis_readyをtureに設定する。
	/// </summary>
	/// <param name="scene"></param>
	static void LoadingThread(SceneLoading* scene);


private:

	std::unique_ptr<TextureAnimation> loading;

	std::unique_ptr<Scene> next_scene{ nullptr };

};