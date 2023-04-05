#include "Graphics.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/Resource/RenderState.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/PostProcess/DeferredLighting.h"
#include "Graphics/PostProcess/PostEffect.h"
#include "Graphics/Effekseer/EffekseerManager.h"
#include "Graphics/Text/TextRenderer.h"


/* --- 定義 ---- */

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

void Graphics::Initialize(ID3D11Device* device, uint32_t screen_width, uint32_t screen_height)
{
	render_state			= std::make_unique<RenderState>();
	deferred_lighting		= std::make_unique<DeferredLighting>();
	post_effect				= std::make_unique<PostEffect>();
	frame_buffer			= std::make_unique<FrameBuffer>();
	effekseer_manager		= std::make_unique<EffekseerManager>();
	constant_buffer_manager	= std::make_unique<ConstantBufferManager>();
	texture_renderer		= std::make_unique<TextureRenderer>();
	text_renderer			= std::make_unique<TextRenderer>();

	deferred_lighting->Initialize(device, screen_width, screen_height);
	post_effect->Initialize(device, screen_width, screen_height);
	frame_buffer->Initialize(device);
	effekseer_manager->Initialize(device);
	texture_renderer->Initialize(device);
	text_renderer->Initialize();
}

void Graphics::EndProcess(ID3D11DeviceContext* dc)
{
	texture_renderer->EndProcess(dc);
	render_state->EndProcess(dc);
	frame_buffer->EndProcess(dc);
	constant_buffer_manager->EndProcess(dc);
	effekseer_manager->EndProcess();
}
