#include "DepthStencil.h"

/* --- ’è‹` ---- */

void DepthStencil::Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	texture.Create(device, width, height, format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL);

	D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = PixelBuffer::GetDSVFormat(format);
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HRESULT hr{ S_OK };
	hr = device->CreateDepthStencilView(texture.GetResource().Get(), &desc, depth_stencil_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void DepthStencil::Create(
	ID3D11Device* device, 
	const D3D11_TEXTURE2D_DESC* tex_desc, 
	const D3D11_SUBRESOURCE_DATA* subresource, 
	const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc, 
	const D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc)
{
	texture.Create(device, tex_desc, subresource, srv_desc);

	HRESULT hr{ S_OK };
	hr = device->CreateDepthStencilView(texture.GetResource().Get(), dsv_desc, depth_stencil_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void DepthStencil::Clear(ID3D11DeviceContext* dc, uint32_t clear_flag, float depth, uint8_t stencil)
{
	dc->ClearDepthStencilView(depth_stencil_view.Get(), clear_flag, depth, stencil);
}
