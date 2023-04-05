#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <tuple>


/* --- 宣言 ---- */

enum class ShaderType
{
	Vertex,		// 頂点シェーダー
	Pixel,		// ピクセルシェーダー
	Geometry,
	Domain,
	Hull,

};

enum class BSMode : uint8_t
{
	None,
	Alpha,		// アルファ合成
	Add,		// 加算合成
	Substract,
	Replace,
	Multiply,
	Lighten,
	Darken,
	Screen,
};

enum class DSMode : uint8_t
{
	Depth_Stencil,
	Depth,
	Stencil,
	None
};

/// <summary>
/// DirectX11自作ヘルパー関数
/// </summary>
class DXHelper
{
public:
	/// <summary>
	/// 深度ステンシルステートの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="depth_stencil_state"></param>
	/// <param name="depth_enable">深度テストを行うか</param>
	/// <param name="stencil_enable">ステンシルテストを行うか</param>
	static void CreateDepthStencilState(ID3D11Device* device, ID3D11DepthStencilState** depth_stencil_state, bool depth_enable, bool stencil_enable);
	/// <summary>
	/// サンプラーステートの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sampler_state"></param>
	/// <param name="filter"></param>
	/// <param name="address_mode">アドレッシングモード</param>
	/// <param name="comparison_func"></param>
	static void CreateSamplerState(ID3D11Device* device, ID3D11SamplerState** sampler_state, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER);
	/// <summary>
	/// ラスタライザステートの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="rasterizer_state"></param>
	/// <param name="fill_mode"></param>
	/// <param name="cull_mode">描画しない面</param>
	/// <param name="is_front_counter_clock_wise">ポリゴンの頂点の順番を逆回りにするか</param>
	static void CreateRasterizerState(ID3D11Device* device, ID3D11RasterizerState** rasterizer_state, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, bool is_front_counter_clock_wise = FALSE);
	/// <summary>
	///  ブレンドステートの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="blend_state"></param>
	/// <param name="blend_mode"></param>
	static void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blend_state, BSMode blend_mode);
	/// <summary>
	/// 各ステートの設定
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="blend_state"></param>
	/// <param name="depth_stencil_state"></param>
	/// <param name="rasterizer_state"></param>
	static void SetState(ID3D11DeviceContext* dc, ID3D11BlendState* blend_state, ID3D11DepthStencilState* depth_stencil_state, ID3D11RasterizerState* rasterizer_state);
	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="constant_buffer"></param>
	/// <param name="buffer_size">16バイトアライメント</param>
	static void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** constant_buffer, UINT buffer_size);
	/// <summary>
	/// 定数バッファの設定
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="constant_buffer"></param>
	/// <param name="slot"></param>
	static void SetConstantBuffers(ID3D11DeviceContext* dc, ID3D11Buffer** constant_buffer, UINT slot);

	static void CreateStructuredBuffer(
		ID3D11Device* device,
		ID3D11Buffer** out_structured_buffer,
		UINT buffer_unit_size, UINT num_elements,
		const void* buffer_subresource_data,
		ID3D11ShaderResourceView** out_structured_shader_resource_view);
	/// <summary>
	/// プリミティブトポロジーの設定
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="before_primitive_topology">設定する前のプリミティブトポロジー</param>
	/// <param name="after_primitive_topology">設定するプリミティブトポロジー</param>
	static void SetPrimitiveTopology(ID3D11DeviceContext* dc, D3D11_PRIMITIVE_TOPOLOGY* before_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY after_primitive_topology);
	/// <summary>
	/// 設定しているブレンドステートの設定を確保する
	/// </summary>
	/// <param name="dc">デバイスコンテキスト</param>
	/// <returns></returns>
	static std::tuple<Microsoft::WRL::ComPtr<ID3D11BlendState>, FLOAT*, UINT> CacheBlendState(ID3D11DeviceContext* dc);
	/// <summary>
	/// レンダーターゲットで映したテクスチャをシェーダーリソースビューで参照するようにバッファを作成する
	/// </summary>
	/// <param name="device"></param>
	/// <param name="screen_width"></param>
	/// <param name="screen_height"></param>
	/// <param name="format">カラーバッファのフォーマット</param>
	/// <param name="out_texture2d">作成したテクスチャ</param>
	/// <param name="out_render_target_view">作成したレンダーターゲットビュー</param>
	/// <param name="out_shader_resource_view">作成したシェーダーリソースビュー</param>
	static void CreateRenderTargetView(ID3D11Device* device, UINT screen_width, UINT screen_height, DXGI_FORMAT format, ID3D11Texture2D** out_texture2d, ID3D11RenderTargetView** out_render_target_view, ID3D11ShaderResourceView** out_shader_resource_view);
	/// <summary>
	/// 深度ステンシルビューの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="screen_width"></param>
	/// <param name="screen_height"></param>
	static void CreateDepthStencilView(ID3D11Device* device, UINT screen_width, UINT screen_height, ID3D11DepthStencilView** out_depth_stencil_view);

};

