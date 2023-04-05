#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <d3d11.h>

#include "Utility/Singleton.h"

/* --- 前方宣言 ---- */

class TextureRenderer;
class RenderState;
class FrameBuffer;
class ConstantBufferManager;
class DeferredLighting;
class PostEffect;
class EffekseerManager;
class TextRenderer;

/* --- 宣言 ---- */

class Graphics : public Singleton<Graphics>
{
public:

	Graphics();
	~Graphics();

	/// <summary>
	/// 各オブジェクトの初期化処理
	/// </summary>
	void Initialize(ID3D11Device* device, uint32_t screen_width, uint32_t screen_height);
	/// <summary>
	/// 各オブジェクトの終了処理
	/// </summary>
	void EndProcess(ID3D11DeviceContext* dc);

	const std::unique_ptr<TextureRenderer>& GetTextureRenderer() const  { return texture_renderer; }
	const std::unique_ptr<RenderState>& GetRenderState() const  { return render_state; }
	const std::unique_ptr<FrameBuffer>& GetFrameBuffer() const  { return frame_buffer; }
	const std::unique_ptr<ConstantBufferManager>& GetConstantBufferManager() const  { return constant_buffer_manager; }
	const std::unique_ptr<DeferredLighting>& GetDeferredLighting() const  { return deferred_lighting; }
	const std::unique_ptr<PostEffect>& GetPostEffect() const  { return post_effect; }
	const std::unique_ptr<EffekseerManager>& GetEffekseerManager() const  { return effekseer_manager; }
	const std::unique_ptr<TextRenderer>& GetTextRenderer() const  { return text_renderer; }

private:

	std::unique_ptr<ConstantBufferManager> constant_buffer_manager; // 定数バッファ管理
	std::unique_ptr<TextureRenderer> texture_renderer;			   // テクスチャを描画
	std::unique_ptr<RenderState> render_state;					   // 描画用ステートを扱う
	std::unique_ptr<FrameBuffer> frame_buffer;					   // レンダーターゲット・デプスステンシル・ビューポートを扱う
	std::unique_ptr<DeferredLighting> deferred_lighting;
	std::unique_ptr<PostEffect> post_effect;
	std::unique_ptr<EffekseerManager> effekseer_manager;
	std::unique_ptr<TextRenderer> text_renderer;
};

