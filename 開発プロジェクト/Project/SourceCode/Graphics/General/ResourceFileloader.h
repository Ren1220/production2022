#pragma once

/* --- 追加のヘッダファイルのインクルード ---- */

#include <string>
#include <sstream>
#include <unordered_map>
#include <d3d11.h>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "Include/misc.h"

/* --- 宣言 ---- */

class ResourceFileLoader
{
public:

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath">ファイルパス</param>
	/// <param name="shader_resource_view">シェーダリソースビュー</param>
	/// <param name="texture2d_desc">テクスチャ情報</param>
	/// <returns>問題がない場合 "S_OK" を返す</returns>
	static HRESULT LoadTexture(ID3D11Device* device, const wchar_t* filepath,
		ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc = nullptr);

	static HRESULT LoadTexture(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shader_resource_view);

	static HRESULT LoadTextureArray(
		ID3D11Device* device, 
		ID3D11DeviceContext* dc, 
		std::vector<std::wstring> wfilepaths, 
		ID3D11ShaderResourceView** shader_resource_view);

	static void ReleaseAllTexture()
	{
		texture_cache.clear();
	}

private:

	static std::unordered_map<std::wstring, ID3D11ShaderResourceView*> texture_cache;

public:

	/// <summary>
	/// ダミーテクスチャの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="value">ピクセルに入力する色 カラーコード(#0xAABBGGRR)</param>
	/// <param name="dimension">何ピクセルで区切るか</param>
	/// <returns>問題がない場合 "S_OK"	 を返す</returns>
	static HRESULT CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view,
		DWORD value /*0xAABBGGRR*/, UINT dimension);

	/// <summary>
	/// ダミーテクスチャの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="value">ピクセルに入力する色 カラーコード(#0xAABBGGRR)</param>
	/// <param name="dimension">何ピクセルで区切るか</param>
	/// <returns>問題がない場合 "S_OK"	 を返す</returns>
	static HRESULT CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view, ID3D11Texture2D** out_texture2d,
		DWORD value /*0xAABBGGRR*/, UINT dimension);

	/// <summary>
	/// 頂点シェーダーの読み込み
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="cso_name">読み込む頂点シェーダーファイル</param>
	/// <param name="vertex_shader">頂点シェーダー</param>
	/// <param name="input_layout">インプットレイアウト</param>
	/// <param name="input_element_desc"></param>
	/// <param name="num_elements">インプットレイアウトデスクの配列数</param>
	/// <returns></returns>
	static HRESULT CreateVSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	/// <summary>
	/// ピクセルシェーダーの読み込み
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="cso_name">読み込みピクセルシェーダーファイル</param>
	/// <param name="pixel_shader">ピクセルシェーダー</param>
	/// <returns></returns>
	static HRESULT CreatePSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	/// <summary>
	/// ジオメトリシェーダーの読み込み
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="cso_name">読み込みジオメトリシェーダーファイル</param>
	/// <param name="geometry_shader">ジオメトリシェーダー</param>
	/// <returns></returns>
	static HRESULT CreateGSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

	static HRESULT CreateHSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);
	static HRESULT CreateDSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);


};