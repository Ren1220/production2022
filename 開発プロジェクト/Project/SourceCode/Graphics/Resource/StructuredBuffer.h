#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>

#include "Include/misc.h"

/* --- 宣言 ---- */

template <class T>
class StructuredBuffer
{
public:

	/// <summary>
	/// バッファの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="subresources">初期化データ、初期化データがない場合はnullptr</param>
	/// <param name="num_elements">データの要素数</param>
	void Create(ID3D11Device* device, const T subresources[], size_t num_elements);
	/// <summary>
	/// バッファのデータの更新、生成時の配列と同じサイズのデータ配列を設定すること。
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="subresources">データ配列</param>
	/// <param name="num_elements">データの要素数</param>
	void UpdateSubresource(ID3D11DeviceContext* dc, const T subresources[], size_t num_elements);

	size_t GetNumElements() const { return num_elements; }
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetBuffer() const { return buffer; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetView() const { return shader_resource_view; }

private:

	// バッファの要素数
	size_t num_elements{};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

template<class T>
inline void StructuredBuffer<T>::Create(
	ID3D11Device* device, 
	const T subresources[],
	size_t num_elements)
{
	this->num_elements = num_elements;
	uint32_t element_unit_size{ static_cast<uint32_t>(sizeof(T)) };

	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(element_unit_size * num_elements);
	desc.StructureByteStride = element_unit_size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;			// シェーダリソースビューでGPUにてデータを参照する
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	if (subresources != nullptr)
	{
		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = subresources;
		data.SysMemPitch = element_unit_size;
		hr = device->CreateBuffer(&desc, &data, buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		hr = device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.NumElements = static_cast<UINT>(num_elements);


	hr = device->CreateShaderResourceView(buffer.Get(), &srv_desc, shader_resource_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

template<class T>
inline void StructuredBuffer<T>::UpdateSubresource(
	ID3D11DeviceContext* dc, 
	const T subresources[], 
	size_t num_elements)
{
	_ASSERT_EXPR(this->num_elements == num_elements, L"生成時に指定した要素数と異なるサイズの配列");

	dc->UpdateSubresource(buffer.Get(), 0, nullptr, subresources, 0, 0);
}
