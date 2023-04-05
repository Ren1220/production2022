#include "ResourceFileloader.h"

#include <fstream>
#include <DDSTextureLoader.h>

std::unordered_map<std::wstring, ID3D11ShaderResourceView*> ResourceFileLoader::texture_cache;

HRESULT ResourceFileLoader::LoadTexture(ID3D11Device* device, const wchar_t* filepath, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;


	std::unordered_map<std::wstring, ID3D11ShaderResourceView*>::const_iterator c_itr = texture_cache.find(filepath);

	// キャッシュに同一パスのデータが存在する場合
	if (c_itr != texture_cache.end())
	{
		// 同一パスのシェーダリソースのポインタを設定する
		*shader_resource_view = c_itr->second;
		// シェーダリソースの参照カウントを加算する
		(*shader_resource_view)->AddRef();
		
		if (texture2d_desc != nullptr)
		{
			// シェーダリソースの情報を取得する
			(*shader_resource_view)->GetResource(resource.GetAddressOf());
		}
	}
	else
	{

		// jpg, png, jpeg　とか
		hr = DirectX::CreateWICTextureFromFile(device, filepath, resource.GetAddressOf(), shader_resource_view);

		if (hr != S_OK)
		{
			// DDS テクスチャ
			hr = DirectX::CreateDDSTextureFromFile(device, filepath, resource.GetAddressOf(), shader_resource_view);
		}

		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// キャッシュにデータを登録する
		texture_cache.emplace(std::make_pair(filepath, *shader_resource_view));
	}
	////QueryInterface : http://nenechi.hatenablog.com/entry/2017/10/05/203115

	// テクスチャ情報を取得する
	if (texture2d_desc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		texture2d->GetDesc(texture2d_desc);
	}

	return hr;
}

HRESULT ResourceFileLoader::LoadTexture(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shader_resource_view)
{
	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	
	// DDS形式のファイルの読み込み
	hr = DirectX::CreateDDSTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shader_resource_view);

	// 失敗したら他の形式のファイルの読み込みを行う
	if (hr != S_OK)
	{
		hr = DirectX::CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return S_OK;
}

HRESULT ResourceFileLoader::LoadTextureArray(
	ID3D11Device* device, 
	ID3D11DeviceContext* dc, 
	std::vector<std::wstring> wfilepaths, 
	ID3D11ShaderResourceView** shader_resource_view)
{

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d_array;
	UINT mip_levels{ 1 };
	const UINT number_of_texture{ static_cast<UINT>(wfilepaths.size()) };
	DXGI_FORMAT texture_format;

	for (UINT i = 0; const auto & wfilepath : wfilepaths)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;

		hr = DirectX::CreateWICTextureFromFileEx(
			device,
			wfilepath.c_str(),
			0ULL,							// ファイルの最大サイズを指定しない
			D3D11_USAGE_STAGING,			// GPU -> CPU へのデータ送信を行うための設定
			0,								// 読み込んだリソースデータは一時的なデータで、このデータは直ぐに解放される
			D3D11_CPU_ACCESS_READ,			// CPUで読み取りを行うための設定
			0,								
			DirectX::WIC_LOADER_DEFAULT,	
			resource.GetAddressOf(),
			nullptr);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// テクスチャデータクラスへのキャスト
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_resource;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture_resource.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		if (i == 0)
		{
			// 先頭のテクスチャデータからテクスチャ配列バッファを作成する
			D3D11_TEXTURE2D_DESC desc{};
			texture_resource->GetDesc(&desc);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.ArraySize = number_of_texture;
			hr = device->CreateTexture2D(&desc, NULL, texture2d_array.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			mip_levels = desc.MipLevels;
			texture_format = desc.Format;
		}

		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		hr = dc->Map(texture_resource.Get(), NULL, D3D11_MAP_READ, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 空テクスチャーのサブリソースをファイルから
		// 画像ファイルを読み込んで作成したテクスチャーの内容で更新する
		for (UINT iMip = 0; iMip < mip_levels; iMip++)
		{
			dc->UpdateSubresource(texture2d_array.Get(),
				D3D11CalcSubresource(iMip, i, mip_levels),
				NULL,
				mapped_subresource.pData,
				mapped_subresource.RowPitch,
				0
			);
		}
		dc->Unmap(texture_resource.Get(), NULL);

		++i;
	}

	// 作成したテクスチャ配列バッファを参照するシェーダーリソースビューを作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = texture_format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // テクスチャ配列をビューするときの設定
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = number_of_texture; // 配列のテクスチャの数
	hr = device->CreateShaderResourceView(texture2d_array.Get(), &desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return S_OK;
}

HRESULT ResourceFileLoader::CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view, DWORD value, UINT dimension)
{
	ID3D11Texture2D* texture2d;
	HRESULT hr{ CreateDummyTexture(device, out_shader_resource_view, &texture2d, value, dimension) };
	texture2d->Release();
	return hr;
}

HRESULT ResourceFileLoader::CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view, ID3D11Texture2D** out_texture2d, DWORD value, UINT dimension)
{
	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = dimension;
	texture2d_desc.Height = dimension;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// ピクセルサイズを計算する
	size_t texels{ dimension * dimension };
	// 各ピクセルにカラーコードの情報を入力する
	std::vector<DWORD> system(texels, value);

	size_t offset = 0;

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = system.data();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, out_texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (out_shader_resource_view != nullptr)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = texture2d_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(*out_texture2d, &shader_resource_view_desc, out_shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}

HRESULT ResourceFileLoader::CreateVSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	FILE* fp{};

	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);


	HRESULT hr = device->CreateVertexShader(
		cso_data.get(),
		cso_sz,
		nullptr,
		vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(
			input_element_desc,
			num_elements,
			cso_data.get(),
			cso_sz,
			input_layout
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	return hr;
}

HRESULT ResourceFileLoader::CreatePSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(
		cso_data.get(),
		cso_sz,
		nullptr,
		pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

HRESULT ResourceFileLoader::CreateGSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(
		cso_data.get(),
		cso_sz,
		nullptr,
		geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

HRESULT ResourceFileLoader::CreateHSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateHullShader(
		cso_data.get(),
		cso_sz,
		nullptr,
		hull_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

HRESULT ResourceFileLoader::CreateDSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateDomainShader(
		cso_data.get(),
		cso_sz,
		nullptr,
		domain_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

#if 0
HRESULT D3D11USER::CreateShaderResourceViewArray(TCHAR* pTextureFileNames[], UINT NumTextures, ID3D11ShaderResourceView** pSRVView)
{
	HRESULT hr = E_FAIL;

	ID3D11Texture2D* pTexture = NULL;

	D3DX11_IMAGE_LOAD_INFO info;
	::ZeroMemory(&info, sizeof(D3DX11_IMAGE_LOAD_INFO));
	info.Width = D3DX11_DEFAULT;
	info.Height = D3DX11_DEFAULT;
	info.Depth = D3DX11_DEFAULT;
	info.FirstMipLevel = D3DX11_DEFAULT;          // テクスチャーの最高解像度のミップマップ レベル。
	info.MipLevels = 1;                           // ミップマップ数。ここでは1つのみとする。
	info.Usage = D3D11_USAGE_STAGING;             // GPU から CPU へのデータ転送 (コピー) をサポート
	info.BindFlags = 0;
	info.CpuAccessFlags = D3D11_CPU_ACCESS_READ;  // リソースをマップ可能にし、CPU がそのリソースの内容を読み取れるようにする
	info.MiscFlags = 0;
	info.Format = DXGI_FORMAT_FROM_FILE;
	info.Filter = D3DX11_FILTER_POINT;            // テクスチャー読み込み時に使用するフィルター
	info.MipFilter = D3DX11_FILTER_POINT;         // ミップマップ作成時に使用するフィルター

	D3D11_TEXTURE2D_DESC desc;
	::ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	for (UINT i = 0; i < NumTextures; i++)
	{
		ID3D11Resource* pLoadedRes = NULL;
		ID3D11Texture2D* pLoadedTexture2D = NULL;

		// 画像ファイルを読み込んでテクスチャーを作成する
		hr = DirectX::CreateWICTextureFromFile(device, pTextureFileNames[i], &info, NULL, &pLoadedRes, NULL);
		if (FAILED(hr))
			goto EXIT;

		hr = pLoadedRes->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast <void**>(&pLoadedTexture2D));
		if (FAILED(hr))
			goto EXIT;


		// 空テクスチャー作成
		if (i == 0)
		{
			// 画像ファイルを読み込んで作成したテクスチャーの情報を取得する
			pLoadedTexture2D->GetDesc(&desc);

			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.ArraySize = NumTextures;
			hr = m_D3DDevice->CreateTexture2D(&desc, NULL, &pTexture);
			if (FAILED(hr))
				goto EXIT;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// ファイルから画像ファイルを読み込んで作成したテクスチャーを、読み込みモードでマップする
		hr = m_D3DDeviceContext->Map(pLoadedTexture2D, NULL, D3D11_MAP_READ, 0, &mappedResource);
		if (FAILED(hr))
			goto EXIT;

		// 空テクスチャーのサブリソースをファイルから画像ファイルを読み込んで作成したテクスチャーの内容で更新する
		for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
		{
			m_D3DDeviceContext->UpdateSubresource(pTexture,
				D3D11CalcSubresource(iMip, i, desc.MipLevels),
				NULL,
				mappedResource.pData,
				mappedResource.RowPitch,
				0
			);
		}
		m_D3DDeviceContext->Unmap(pLoadedTexture2D, NULL);

		SAFE_RELEASE(pLoadedTexture2D);
		SAFE_RELEASE(pLoadedRes);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = NumTextures;
	// シェーダーリソースビューを作成する
	hr = m_D3DDevice->CreateShaderResourceView(pTexture, &SRVDesc, pSRVView);
	if (FAILED(hr))
		goto EXIT;

	hr = S_OK;

EXIT:
	SAFE_RELEASE(pTexture);

	return hr;
}
#endif