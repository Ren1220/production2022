#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "Scene.h"

#include "Arithmetic/Arithmetic.h"
#include "Audio/AudioHandle.h"

#include "Graphics/Resource/DepthStencil.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/Camera/PerspectiveCamera.h"

#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Model/GLTFSkinnedMesh.h"

/* --- 宣言 ---- */

class SceneGame : public Scene
{
public:
	SceneGame();
	~SceneGame() override = default;

public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;
	void ImGuiRender() override;

	void GameRetry(ID3D11Device* device);

private:

	/// <summary>
	/// 画面解像度を変更した際に行う処理。
	/// フレームバッファのサイズを変更し再生成する。
	/// </summary>
	void ResetScreen();

	bool is_game_retry;

	std::shared_ptr<RenderTarget> main_render_target; // メインレンダーターゲット
	std::shared_ptr<DepthStencil> main_depth_stencil; // メインレンダーターゲット


	AudioHandle bgm;

#ifdef _DEBUG
	void ControlCamera(float elapsed_time);

	void ImGuiScene();
	void ImGUiCameraControl();

	bool can_control_camera;
	std::shared_ptr<Camera> camera;
#endif // _DEBUG
};
