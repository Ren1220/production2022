#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Texture/Texture.h"

/* --- 宣言 ---- */

/// <summary>
/// 基本的にはGraphicを介してレンダーターゲットを設定し、
/// その工程の中でGraphicに設定したいレンダーターゲットのポインタを保持させるため、
/// shared_ptrで生成する。
/// </summary>
class RenderTarget
{
public:

	RenderTarget() : render_target_view(), texture() {}
	~RenderTarget() = default;

	/// <summary>
	/// 使用するフォーマットはシェーダリソースとして参照するときのフォーマットを指定する。
	/// </summary>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT color_format);
	
	/// <summary>
	/// すべて自身で設定するとき
	/// </summary>
	void Create(ID3D11Device* device,
				const D3D11_TEXTURE2D_DESC* tex_desc,
				const D3D11_SUBRESOURCE_DATA* subresource,
				const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc,
				const D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc);

	void Clear(ID3D11DeviceContext* dc, const float clear_color[4] = nullptr);

	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetView() { return render_target_view; }
	const Texture& GetTexture()										{ return texture; }

private:

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Texture texture;
};
