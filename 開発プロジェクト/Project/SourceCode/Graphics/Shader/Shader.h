#pragma once

/* --- ヘッダーファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <map> // サンプラーステートを保持する配列ソートするためこっち

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/Resource/RenderState.h"

/* --- 宣言 ---- */

/// <summary>
/// シェーダー(VS,HS,DS,GS,PS)、描画ステート(Blend,Rasterizer,DepthStencil,Sampler)を設定する。
/// 描画ステートに関しては、リソースの無駄を避けるため、重複した設定のステートは他のシェーダークラスと共有される。
/// </summary>
class Shader
{
public:

	Shader();
	virtual ~Shader();

	virtual void Create(ID3D11Device* device) {};

	/// <summary>
	/// 頂点シェーダーの作成。対応したInputLayoutがない場合はここで作成する必要がある。
	/// </summary>
	/// <param name="filepath">拡張子まで</param>
	void CreateVSFromCSO(ID3D11Device* device, const char* filepath, 
						 ID3D11InputLayout** input_layout = nullptr, D3D11_INPUT_ELEMENT_DESC* desc = nullptr, UINT num = NULL);

	/// <summary>
	/// ハルシェーダーの作成
	/// </summary>
	void CreateHSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// ドメインシェーダーの作成
	/// </summary>
	void CreateDSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// ジオメトリシェーダーの作成
	/// </summary>
	void CreateGSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// ピクセルシェーダーの作成
	/// </summary>
	void CreatePSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// ブレンドステートの作成
	/// </summary>
	void CreateBlendState(ID3D11Device* device, BSMode bs_mode);

	/// <summary>
	/// 深度ステンシルステートの作成
	/// </summary>
	void CreateDepthStencilState(ID3D11Device* device, DSMode ds_mode);

	/// <summary>
	/// ラスタライザステートの作成
	/// </summary>
	void CreateRasterizerState(
		ID3D11Device* device, 
		D3D11_FILL_MODE fill_mode, 
		D3D11_CULL_MODE cull_mode = D3D11_CULL_BACK,			
		bool is_front_counter_clock_wise = false);  

	/// <summary>
	/// サンプラーステートの作成。他の描画ステートと異なり、シェーダーにバインドする際のインデックスをkeyとしてmap配列に生成したステートを挿入する。
	/// </summary>
	/// <param name="slot_num">シェーダーで使用する際の番号</param>
	void CreateSamplerState(
		ID3D11Device* device,
		UINT slot_num,
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER);

	/// <summary>
	/// シェーダーをデバイスコンテキストに設定する。
	/// 同時に描画ステートのバインドも行う。
	/// </summary>
	virtual void Activate(ID3D11DeviceContext* dc) ;

	/// <summary>
	/// 設定したシェーダーと描画ステートを解除する。
	/// </summary>
	virtual void Deactivate(ID3D11DeviceContext* dc);

	const Microsoft::WRL::ComPtr<ID3D11VertexShader>&   GetVertexShader()   { return vertex_shader; }
	const Microsoft::WRL::ComPtr<ID3D11PixelShader>&    GetPixelShader()	{ return pixel_shader; }
	const Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetGeometryShader() { return geometry_shader; }
	const Microsoft::WRL::ComPtr<ID3D11HullShader>&     GetHullShader()     { return hull_shader; }
	const Microsoft::WRL::ComPtr<ID3D11DomainShader>&   GetDomainShader()   { return domain_shader; }

protected:

	virtual void ActivateRenderState(ID3D11DeviceContext* dc);

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11HullShader>		hull_shader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		domain_shader;

	Microsoft::WRL::ComPtr<ID3D11BlendState>				   blend_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>			   depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>			   rasterizer_state;
	std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>> sampler_states;

};