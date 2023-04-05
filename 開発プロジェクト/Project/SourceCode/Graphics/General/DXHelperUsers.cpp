#include "DXHelperUsers.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <crtdbg.h>

#include "Include/misc.h"

/* --- 関数の定義 ---- */

void DXHelper::CreateDepthStencilState(ID3D11Device* device, ID3D11DepthStencilState** depth_stencil_state, bool depth_enable, bool stencil_enable)
{
	D3D11_DEPTH_STENCIL_DESC desc{};
	
	if (desc.DepthEnable = depth_enable)
	{
		// 深度テストを行う

		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}

	if (desc.StencilEnable = stencil_enable)
	{
		// ステンシルテストを行う

		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}

	HRESULT hr = device->CreateDepthStencilState(&desc, depth_stencil_state);
	_ASSERT_EXPR(SUCCEEDED(hr), L"深度ステンシルステートの作成に失敗しました");
}

void DXHelper::CreateSamplerState(ID3D11Device* device, ID3D11SamplerState** sampler_state, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func)
{
	D3D11_SAMPLER_DESC desc{};

	desc.Filter = filter;
	desc.AddressU = desc.AddressV = desc.AddressW = address_mode;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 16;
	desc.ComparisonFunc = comparison_func;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&desc, sampler_state);
	_ASSERT_EXPR(SUCCEEDED(hr), L"サンプラーステートの作成に失敗しました");
}

void DXHelper::CreateRasterizerState(ID3D11Device* device, ID3D11RasterizerState** rasterizer_state, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, bool is_front_counter_clock_wise)
{
	D3D11_RASTERIZER_DESC desc{};

	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;

	desc.FillMode = fill_mode;
	desc.CullMode = cull_mode;
	desc.FrontCounterClockwise = is_front_counter_clock_wise;

	HRESULT hr = device->CreateRasterizerState(&desc, rasterizer_state);
	_ASSERT_EXPR(SUCCEEDED(hr), L"ラスタライザステートの作成に失敗しました");
}

void DXHelper::CreateBlendState(ID3D11Device* device, ID3D11BlendState** blend_state, BSMode blend_mode)
{
	D3D11_BLEND_DESC desc{};

	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	switch (blend_mode)
	{
	case BSMode::None:
		desc.RenderTarget[0].BlendEnable = false;
		break;
	case BSMode::Alpha:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Add:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Substract:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Replace:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Multiply:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Lighten:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Darken:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BSMode::Screen:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	HRESULT hr = device->CreateBlendState(&desc, blend_state);
	_ASSERT_EXPR(SUCCEEDED(hr), L"ブレンドステートの作成に失敗しました");
}

void DXHelper::SetState(ID3D11DeviceContext* dc, ID3D11BlendState* blend_state, ID3D11DepthStencilState* depth_stencil_state, ID3D11RasterizerState* rasterizer_state)
{
	// ブレンドステートの設定
	dc->OMSetBlendState(blend_state, nullptr, 0xFFFFFFFF);
	// 深度ステンシルステートの設定
	dc->OMSetDepthStencilState(depth_stencil_state, 0);
	// ラスタライザステートの設定
	dc->RSSetState(rasterizer_state);
}

void DXHelper::CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** constant_buffer, UINT buffer_size)
{
	// バッファの設定
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = buffer_size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// バッファの作成
	HRESULT hr = device->CreateBuffer(&desc, nullptr, constant_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), L"定数バッファの作成に失敗しました");
}

void DXHelper::SetConstantBuffers(ID3D11DeviceContext* dc, ID3D11Buffer** constant_buffer, UINT slot)
{
	dc->VSSetConstantBuffers(slot, 1, constant_buffer);
	dc->GSSetConstantBuffers(slot, 1, constant_buffer);
	dc->PSSetConstantBuffers(slot, 1, constant_buffer);
	dc->HSSetConstantBuffers(slot, 1, constant_buffer);
	dc->DSSetConstantBuffers(slot, 1, constant_buffer);
}

void DXHelper::CreateStructuredBuffer(
	ID3D11Device* device,
	ID3D11Buffer** out_structured_buffer,
	UINT buffer_unit_size, UINT num_elements,
	const void* buffer_subresource_data,
	ID3D11ShaderResourceView** out_structured_shader_resource_view)
{
	HRESULT hr{ S_OK };

	// バッファの作成
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = buffer_unit_size * num_elements;
		desc.StructureByteStride = buffer_unit_size;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		if (buffer_subresource_data != nullptr)
		{
			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = buffer_subresource_data;
			data.SysMemPitch = buffer_unit_size;
			hr = device->CreateBuffer(&desc, &data, out_structured_buffer);
		}
		else
		{
			hr = device->CreateBuffer(&desc, nullptr, out_structured_buffer);
		}

		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// シェーダーリソースビューの作成 
	// -シェーダー側でバッファのデータを読み取る際、
	//  シェーダリソースビューを介する
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = num_elements;

		hr = device->CreateShaderResourceView(*out_structured_buffer, &desc, out_structured_shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void DXHelper::SetPrimitiveTopology(ID3D11DeviceContext* dc, D3D11_PRIMITIVE_TOPOLOGY* before_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY after_primitive_topology)
{
	dc->IAGetPrimitiveTopology(before_primitive_topology);
	dc->IASetPrimitiveTopology(after_primitive_topology);
}

std::tuple<Microsoft::WRL::ComPtr<ID3D11BlendState>, FLOAT*, UINT> DXHelper::CacheBlendState(ID3D11DeviceContext* dc)
{
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
	FLOAT factor[4];
	UINT mask;
	dc->OMGetBlendState(blend_state.GetAddressOf(), factor, &mask);
	return { blend_state, factor, mask };
}

void DXHelper::CreateRenderTargetView(ID3D11Device* device, UINT screen_width, UINT screen_height, DXGI_FORMAT format, ID3D11Texture2D** out_texture2d, ID3D11RenderTargetView** out_render_target_view, ID3D11ShaderResourceView** out_shader_resource_view)
{
	HRESULT hr{ S_OK };

	// テクスチャの作成
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = screen_width;
	texture2d_desc.Height = screen_height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, nullptr, out_texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// レンダーターゲットビューの作成
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
	render_target_view_desc.Format = texture2d_desc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(*out_texture2d, &render_target_view_desc, out_render_target_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダーリソースビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(*out_texture2d, &shader_resource_view_desc, out_shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void DXHelper::CreateDepthStencilView(ID3D11Device* device, UINT screen_width, UINT screen_height, ID3D11DepthStencilView** out_depth_stencil_view)
{
	HRESULT hr{ S_OK };

	// 深度ステンシル設定
	Microsoft::WRL::ComPtr<ID3D11Texture2D> resource;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	ZeroMemory(&texture2d_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2d_desc.Width = screen_width;
	texture2d_desc.Height = screen_height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	hr = device->CreateTexture2D(&texture2d_desc, NULL, resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = texture2d_desc.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = device->CreateDepthStencilView(resource.Get(), &dsvd, out_depth_stencil_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

