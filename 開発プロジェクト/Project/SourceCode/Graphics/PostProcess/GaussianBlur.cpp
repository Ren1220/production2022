#include "GaussianBlur.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- 定義 ---- */

GaussianBlur::GaussianBlur()
	: width(), height(), weights()
{
}

void GaussianBlur::Initialize(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;

	blur_x_render_target = std::make_shared<RenderTarget>();
	blur_x_render_target->Create(device, width, height, format);
	blur_y_render_target = std::make_shared<RenderTarget>();
	blur_y_render_target->Create(device, width, height, format);

	{
		blur_x_shader = std::make_unique<Shader>();
		blur_y_shader = std::make_unique<Shader>();

		blur_x_shader->CreateVSFromCSO(device, "Shader/GausBlurXVS.cso");
		blur_x_shader->CreatePSFromCSO(device, "Shader/GausBlurPS.cso");
		blur_y_shader->CreateVSFromCSO(device, "Shader/GausBlurYVS.cso");
		blur_y_shader->CreatePSFromCSO(device, "Shader/GausBlurPS.cso");

		// アドレスボーダー、リニア補間
		blur_x_shader->CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
		blur_y_shader->CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
	}

	// ブラーの重みテーブル
	weight_cb.Create(device);
}

void GaussianBlur::CalcWeightTable(float distribution)
{
	// 重みの合計を記録する変数を定義する
	float total = 0;

	// ここからガウス関数を用いて重みを計算している
	// ループ変数のxが基準テクセルからの距離
	for (int x = 0; x < num_weights; x++)
	{
		weights[x] = expf(-0.5f * (float)(x * x) / distribution);
		total += 2.0f * weights[x];
	}

	// 重みの合計で除算することで、重みの合計を1にしている
	for (int i = 0; i < num_weights; i++)
	{
		weights[i] /= total;
	}
}

void GaussianBlur::Apply(
	ID3D11DeviceContext* dc, 
	const Texture& texture, 
	float distribution)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer()};
	auto& graphics{ Graphics::GetInstance() };

	// 定数バッファの更新
	{
		CBufferWeight cb;
		CalcWeightTable(distribution);
		memcpy_s(&cb, sizeof(CBufferWeight), weights.data(), sizeof(float) * num_weights);
		weight_cb.UpdateSubresouce(dc, cb);

		// 定数バッファを設定
		graphics.GetConstantBufferManager()->SetBuffer(dc, CBufferWeight::slot, &weight_cb);
	}

	// 横ブラーをかける
	{
		// レンダーターゲットの設定
		frame_buffer->Set(
			dc,
			static_cast<float>(width),
			static_cast<float>(height),
			nullptr,
			0,
			blur_x_render_target);

		blur_x_shader->Activate(dc);
		graphics.GetTextureRenderer()->Draw(dc, width, height, texture);
	}

	// 横ブラーをかけたテクスチャに縦ブラーをかける
	{
		// レンダーターゲットの設定
		frame_buffer->Set(
			dc,
			static_cast<float>(width),
			static_cast<float>(height),
			nullptr,
			0,
			blur_y_render_target);

		blur_y_shader->Activate(dc);
		graphics.GetTextureRenderer()->Draw(dc, width, height, blur_x_render_target->GetTexture());
	}
}