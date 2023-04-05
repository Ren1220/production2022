#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <stdint.h>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>

#include "Scene.h"

#include "Arithmetic/Arithmetic.h"

#include "Graphics/Resource/Texture/Texture.h"
#include "Graphics/Resource/DepthStencil.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Shader/Sprite2DShader.h"

/* --- 宣言 ---- */

class SceneTitle : public Scene
{
public:
	SceneTitle();
	~SceneTitle() override = default;

public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;
	void ImGuiRender() override;


private:

	/// <summary>
	/// 画面サイズを変更した際に行う処理
	/// </summary>
	void ResetScreen();

	std::shared_ptr<RenderTarget> main_render_target; // メインレンダーターゲット
	std::shared_ptr<DepthStencil> main_depth_stencil; // メインレンダーターゲット

	Sprite2DShader sprite_shader;
	Shader deferred_shader;

};
