#include "Sprite2DShader.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../General/ResourceFileloader.h"

/* --- 関数の実装 ---- */

void Sprite2DShader::Create(ID3D11Device* device)
{
	// 頂点シェーダー
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ResourceFileLoader::CreateVSfromCSO(device, "Shader/Sprite2DVS.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		ResourceFileLoader::CreatePSfromCSO(device, "Shader/SpritePS.cso", pixel_shader.GetAddressOf());

		CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_BACK, false);
		CreateSamplerState(device, 0);
		CreateBlendState(device, BSMode::Alpha);

	}

	// 定数バッファ
	material_cb.Create(device);

	// 仮のテクスチャ
	dummy_texture = std::make_unique<Texture>();
	dummy_texture->Create(device);
}

void Sprite2DShader::Activate(ID3D11DeviceContext* dc)
{
	Activate(dc, nullptr, { 1, 1, 1, 1 });
}

void Sprite2DShader::Activate(ID3D11DeviceContext* dc, const Vector4& material_color)
{
	Activate(dc, nullptr, material_color);
}

void Sprite2DShader::Activate(ID3D11DeviceContext* dc, const Texture* texture, const Vector4& material_color)
{

	if (texture)
	{
		dc->PSSetShaderResources(0, 1, texture->GetView().GetAddressOf());
	}
	else
	{
		dc->PSSetShaderResources(0, 1, dummy_texture->GetView().GetAddressOf());
	}

	dc->UpdateSubresource(material_cb.Get(), 0, 0, &material_color, 0, 0);
	// ４番に設定
	dc->PSSetConstantBuffers(4, 1, material_cb.GetAddressOf());

	dc->IASetInputLayout(input_layout.Get());

	Shader::Activate(dc);
}

