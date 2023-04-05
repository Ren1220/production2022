#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <unordered_map>

#include "Include/misc.h"
#include "../PixelBuffer.h" // カラーフォーマットから対応するフォーマットを決定する。

/* --- 宣言 ---- */

class Texture
{
public:

	Texture();
	virtual ~Texture();

	/// <summary>
	/// ファイルのテクスチャを読み込む、同一パスのテクスチャが既に読み込まれている場合、キャッシュデータを読み込む。
	/// </summary>
	void Create(ID3D11Device* device, const wchar_t* filepath);
	/// <summary>
	/// 色を指定してテクスチャを作成
	/// </summary>
	void Create(ID3D11Device* device, DWORD color = 0xffffffff);
	/// <summary>
	/// 引数のビューを設定する。ビューから他の情報も取得する。
	/// </summary>
	void Create(ID3D11ShaderResourceView** src);
	/// <summary>
	/// 引数のデータからテクスチャ、ビューを作成する。
	/// </summary>
	void Create(
		ID3D11Device* device, 
		uint32_t width, 
		uint32_t height, 
		DXGI_FORMAT format, 
		uint32_t bind_flag);
	/// <summary>
	/// 引数のデータからテクスチャ、ビューを作成する。
	/// </summary>
	void Create(
		ID3D11Device* device, 
		const D3D11_TEXTURE2D_DESC* tex_desc, 
		const D3D11_SUBRESOURCE_DATA* subresource, 
		const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc);

	const D3D11_TEXTURE2D_DESC& GetDesc()							  const { return desc; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetView() const { return shader_resoruce_view; }
	const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetResource()	  const { return resource; }

	static void ClearCaches() { resource_view_caches.clear(); }

private:

	static std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resource_view_caches;

protected:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resoruce_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> resource;

	D3D11_TEXTURE2D_DESC desc;
};
