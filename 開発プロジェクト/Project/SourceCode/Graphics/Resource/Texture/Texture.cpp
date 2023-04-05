#include "Texture.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <WICTextureLoader.h> // テクスチャ読み込み

/* --- 定義 ---- */

std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Texture::resource_view_caches = {};

Texture::Texture()
	: shader_resoruce_view(), resource(), desc() 
{

}

Texture::~Texture()
{
}

void Texture::Create(ID3D11Device* device, const wchar_t* filepath)
{

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> rsrc{};

	const auto& found_view{ resource_view_caches.find(filepath) };

	if (found_view != resource_view_caches.end())
	{
		// キャッシュデータが存在する
		//shader_resoruce_view = found_view->second;
		hr = found_view->second.CopyTo(shader_resoruce_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



		shader_resoruce_view->GetResource(rsrc.GetAddressOf());
	}
	else
	{

		// キャッシュデータが存在しない
		hr = DirectX::CreateWICTextureFromFile(
			device, 
			filepath, 
			rsrc.ReleaseAndGetAddressOf(), 
			shader_resoruce_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// キャッシング
		hr = shader_resoruce_view.CopyTo(resource_view_caches[filepath].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// キャスト後ポインタのコピー
	hr = rsrc->QueryInterface<ID3D11Texture2D>(resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// テクスチャの情報を取得
	resource->GetDesc(&desc);
}

void Texture::Create(ID3D11Device* device, DWORD color)
{
	HRESULT hr{ S_OK };

	desc.Width = 1;
	desc.Height = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &color;
	data.SysMemPitch = sizeof(DWORD);

	hr = device->CreateTexture2D(&desc, &data, resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(resource.Get(), &srv_desc, shader_resoruce_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Texture::Create(ID3D11ShaderResourceView** src)
{
	// ビューのコピー
	shader_resoruce_view = *src;

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> rsrc;
	// キャスト後ポインタのコピー
	shader_resoruce_view->GetResource(rsrc.ReleaseAndGetAddressOf());
	hr = rsrc->QueryInterface<ID3D11Texture2D>(resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// テクスチャの情報を取得
	resource->GetDesc(&desc);
}

void Texture::Create(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height,
	DXGI_FORMAT format, 
	uint32_t bind_flag)
{
	HRESULT hr{ S_OK };

	desc.Width = width;
	desc.Height = height;
	desc.Format = PixelBuffer::GetBaseFormat(format);
	desc.BindFlags = bind_flag;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = 0;

	hr = device->CreateTexture2D(&desc, nullptr, resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(resource.Get(), &srv_desc, shader_resoruce_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Texture::Create(
	ID3D11Device* device, 
	const D3D11_TEXTURE2D_DESC* tex_desc, 
	const D3D11_SUBRESOURCE_DATA* subresource, 
	const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc)
{
	HRESULT hr{ S_OK };

	// テクスチャ情報をコピー
	memcpy_s(&desc, sizeof(desc), tex_desc, sizeof(*tex_desc));

	hr = device->CreateTexture2D(&desc, subresource, resource.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateShaderResourceView(resource.Get(), srv_desc, shader_resoruce_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
