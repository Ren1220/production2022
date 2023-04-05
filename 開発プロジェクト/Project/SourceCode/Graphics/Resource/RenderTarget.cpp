#include "RenderTarget.h"

/* --- ’è‹` ---- */

void RenderTarget::Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT color_format)
{
	texture.Create(device, width, height, color_format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

	D3D11_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = color_format;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	HRESULT hr{ S_OK };
	hr = device->CreateRenderTargetView(texture.GetResource().Get(), &desc, render_target_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void RenderTarget::Create(
	ID3D11Device* device, 
	const D3D11_TEXTURE2D_DESC* tex_desc, 
	const D3D11_SUBRESOURCE_DATA* subresource, 
	const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc, 
	const D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc)
{
	texture.Create(device, tex_desc, subresource, srv_desc);

	HRESULT hr{ S_OK };
	hr = device->CreateRenderTargetView(texture.GetResource().Get(), rtv_desc, render_target_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void RenderTarget::Clear(ID3D11DeviceContext* dc, const FLOAT clear_color[4])
{
	constexpr float default_clear_color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

	if (clear_color != nullptr)
	{
		dc->ClearRenderTargetView(render_target_view.Get(), clear_color);
	}
	else
	{
		dc->ClearRenderTargetView(render_target_view.Get(), default_clear_color);
	}
}
