#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Texture/Texture.h"

/* --- 宣言 ---- */

/// <summary>
/// 基本的にはGraphicを介してデプスステンシルを設定し、
/// その工程の中でGraphicに設定したいデプスステンシルのポインタを保持させるため、
/// shared_ptrで生成する。
/// </summary>
class DepthStencil
{
public:

	DepthStencil() : depth_stencil_view(), texture() {}
	~DepthStencil() = default;

	/// <summary>
	/// 使用するフォーマットはシェーダリソースとして参照するときのフォーマットを指定する。
	/// 内部でそのフォーマットに対応した深度ステンシルビューのフォーマットを使用する。
	/// </summary>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT color_format);

	/// <summary>
	/// すべて自身で設定するとき
	/// </summary>
	void Create(ID3D11Device* device,
		const D3D11_TEXTURE2D_DESC* tex_desc,
		const D3D11_SUBRESOURCE_DATA* subresource,
		const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc,
		const D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc);

	void Clear(ID3D11DeviceContext* dc, uint32_t clear_flag, float depth = 1.0f, uint8_t stencil = 0);

	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetView() const { return depth_stencil_view; }
	const Texture& GetTexture()									    const { return texture; }

private:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Texture texture;
};
