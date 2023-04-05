#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <unordered_map>

#include "Graphics/General/DXHelperUsers.h"

/* --- 宣言 ---- */

class RenderState
{
public:

	RenderState() :
		blend_states(),
		depth_stencil_states(),
		rasterizer_states(),
		sampler_states() {}

	virtual ~RenderState() = default;

	void EndProcess(ID3D11DeviceContext* dc);

	// 各ステートを生成する。引数をインデックスに生成したステートを登録する。
	// （複数引数があるステートはビットシフトで一つのインデックスにする）

	void CreateBlendState(
		ID3D11Device* device, 
		BSMode bs_mode, 
		ID3D11BlendState** out_bs);

	void CreateDepthStencilState(
		ID3D11Device* device, 
		DSMode ds_mode,
		ID3D11DepthStencilState** out_ds);

	void CreateRasterizerState(
		ID3D11Device* device, 
		D3D11_FILL_MODE fill,
		D3D11_CULL_MODE cull,
		bool is_front_counter_clock_wise, 
		ID3D11RasterizerState** out_rs);

	void CreateSamplerState(
		ID3D11Device* device,
		D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE address_mode,
		D3D11_COMPARISON_FUNC comparison_func,
		ID3D11SamplerState** out_ss);

private:

	std::unordered_map<BSMode,   Microsoft::WRL::ComPtr<ID3D11BlendState>>		  blend_states;
	std::unordered_map<DSMode,   Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> depth_stencil_states;
	std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D11RasterizerState>>   rasterizer_states;
	std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D11SamplerState>>	  sampler_states;
};
