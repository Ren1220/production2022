#pragma once

// 2022/08/06

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>

#include "Include/misc.h"

/* --- 宣言 ---- */

class ConstantBufferBase
{
public:

	ConstantBufferBase()
		: buffer() {};
	virtual ~ConstantBufferBase() = default;

	ID3D11Buffer* Get() const { return buffer.Get(); }
	ID3D11Buffer*const* GetAddressOf() const { return buffer.GetAddressOf(); }

	virtual const char* GetName()  const = 0;
	virtual const size_t GetHash() const = 0;

protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

template <class T>
class ConstantBuffer : public ConstantBufferBase
{
public:

	ConstantBuffer()
		: ConstantBufferBase() {}
	~ConstantBuffer() = default;

	const char* GetName() const override { return typeid(*this).name(); }
	/// <summary>
	/// TypeIDクラスのハッシュコードは使わずクラス名をハッシュコードしたものを使う。
	/// </summary>
	/// <returns></returns>
	const size_t GetHash() const override { return std::hash<std::string>()(typeid(*this).name()); }

	void Create(ID3D11Device* device, const T* subresource = nullptr);

	void UpdateSubresouce(ID3D11DeviceContext* dc, const T& subreosuce) const;
};

template<class T>
void ConstantBuffer<T>::Create(ID3D11Device* device, const T* subresource)
{
	UINT byte_width{ sizeof(T) };
	_ASSERT_EXPR(byte_width % 16 == 0 && byte_width != 0, L"16byteアラインメントに違反しています。");

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(T);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	
	if (subresource != NULL)
	{
		D3D11_SUBRESOURCE_DATA data{};

		data.pSysMem = subresource;

		HRESULT hr = device->CreateBuffer(&desc, &data, buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		HRESULT hr = device->CreateBuffer(&desc, nullptr, buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

template<class T>
inline void ConstantBuffer<T>::UpdateSubresouce(ID3D11DeviceContext* dc, const T& subresource) const
{
	dc->UpdateSubresource(buffer.Get(), 0, 0, &subresource, 0, 0);
}

