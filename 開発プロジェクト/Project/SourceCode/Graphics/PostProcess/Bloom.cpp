#include "Bloom.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- 定義 ---- */

void Bloom::Initialize(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;


	// 輝度抽出バッファ
	{
		// レンダーターゲット
		luminance_render_target = std::make_shared<RenderTarget>();
		luminance_render_target->Create(device, width, height, format);

		// シェーダー
		luminance_shader = std::make_unique<Shader>();
		luminance_shader->CreateVSFromCSO(device, "Shader/LuminanceSamplingVS.cso");
		luminance_shader->CreatePSFromCSO(device, "Shader/LuminanceSamplingPS.cso");

		luminance_shader->CreateBlendState(device, BSMode::Add);
	}
	
	// ブラー合成
	{
		// レンダーターゲット
		blur_composition_render_target = std::make_shared<RenderTarget>();
		blur_composition_render_target->Create(device, width, height, format);

		// シェーダー
		blur_composition_shader = std::make_unique<Shader>();
		blur_composition_shader->CreateVSFromCSO(device, "Shader/BlurCompositionVS.cso");
		blur_composition_shader->CreatePSFromCSO(device, "Shader/BlurCompositionPS.cso");

		blur_composition_shader->CreateBlendState(device, BSMode::Add);
	}

	// ガウスブラー
	// ブラーをかけたテクスチャに再びブラーをかける。
	// 加えてテクスチャをあえて引き延ばして描画することで、
	// 処理するピクセルの数を減らす
	for (size_t bi = 0; bi < blur_samples.size(); ++bi)
	{
		uint32_t w{ width  / (1 << bi) }; // width  / 2^bi
		uint32_t h{ height / (1 << bi) }; // height / 2^bi

		blur_samples[bi].Initialize(device, w, h, format);
	}
}

void Bloom::Apply(ID3D11DeviceContext* dc, const Texture& scene_texture)
{
	// テクスチャの輝度を抽出する
	SamplingLuminance(dc, scene_texture);

	// 抽出したテクスチャに複数回ブラーをかける
	ApplyBlurEffect(dc);

	// 作成したブラーテクスチャを合成する
	CompositionBlur(dc);
}

void Bloom::SamplingLuminance(ID3D11DeviceContext* dc, const Texture& scene_texture)
{
	auto& graphics{ Graphics::GetInstance() };
	auto& frame_buffer{ graphics.GetFrameBuffer()};
	auto& texture_renderer{ graphics.GetTextureRenderer() };

	// レンダーターゲットを変更する
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		luminance_render_target);

	texture_renderer->BeginDraw(dc, luminance_shader.get());
	// 描画処理を行い輝度を抽出したテクスチャを作成する
	texture_renderer->Draw(dc, scene_texture);
}

void Bloom::ApplyBlurEffect(ID3D11DeviceContext* dc)
{
	// 輝度を抽出したテクスチャに複数回ブラーをかける
	{
		blur_samples[0].Apply(dc, luminance_render_target->GetTexture());

		for (uint32_t bi = 1U; bi < blur_sample_num; bi++)
		{
			// ブラーをかけたテクスチャに再びブラーをかける。
			// 加えてテクスチャをあえて引き延ばして描画することで、
			// 処理するピクセルの数を減らす
			blur_samples[bi].Apply(dc, blur_samples[static_cast<size_t>(bi) - 1ULL].GetTexture());
		}
	}
}

void Bloom::CompositionBlur(ID3D11DeviceContext* dc)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer()};
	auto& graphics{ Graphics::GetInstance() };

	// レンダーターゲットを変更する
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		blur_composition_render_target);

	std::array<ID3D11ShaderResourceView*, blur_sample_num> srvs{};
	for (uint32_t bi = 0; bi < blur_sample_num; ++bi)
	{
		srvs[bi] = blur_samples[bi].GetTexture().GetView().Get();
	}

	graphics.GetTextureRenderer()->BeginDraw(dc, blur_composition_shader.get());
	// 描画処理を行い輝度を抽出したテクスチャを作成する
	graphics.GetTextureRenderer()->Draw(
		dc, 
		srvs.data(),
		static_cast<uint32_t>(srvs.size()), 
		static_cast<float>(width),
		static_cast<float>(height));
}

