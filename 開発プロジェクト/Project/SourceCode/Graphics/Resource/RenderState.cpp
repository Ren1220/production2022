#include "RenderState.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Include/misc.h"


/* --- 定義 ---- */

void RenderState::EndProcess(ID3D11DeviceContext* dc)
{
	blend_states.clear();
	depth_stencil_states.clear();
	rasterizer_states.clear();
	sampler_states.clear();
}

void RenderState::CreateBlendState(
	ID3D11Device* device,
	BSMode bs_mode, 
	ID3D11BlendState** out_blend_state)
{
	HRESULT hr{ S_OK };
	const auto found{ blend_states.find(bs_mode) };

	if (found != blend_states.end())
	{
		hr = found->second.CopyTo(out_blend_state);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		// 作成したステートは配列に登録される。
		// 新しい設定のステートを作成し、そのアドレスをコピーする。
		auto& create_bs{ blend_states[bs_mode] };
		DXHelper::CreateBlendState(device, create_bs.ReleaseAndGetAddressOf(), bs_mode);
		hr = create_bs.CopyTo(out_blend_state);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void RenderState::CreateDepthStencilState(
	ID3D11Device* device,
	DSMode ds_mode,
	ID3D11DepthStencilState** out_depth_stencil_state)
{
	HRESULT hr{ S_OK };
	auto found{ depth_stencil_states.find(ds_mode) };

	if (found != depth_stencil_states.end())
	{
		hr = found->second.CopyTo(out_depth_stencil_state);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		// 作成したステートは配列に登録される。
		// 新しい設定のステートを作成し、そのアドレスをコピーする。
		auto& create_ds{ depth_stencil_states[ds_mode] };

		bool d, s;
		switch (ds_mode)
		{
		case DSMode::Depth_Stencil: d = true;  s = true;  break;
		case DSMode::Depth:         d = true;  s = false; break;
		case DSMode::Stencil:       d = false; s = true;  break;
		case DSMode::None: default: d = false; s = false; break;
		}

		DXHelper::CreateDepthStencilState(device, create_ds.ReleaseAndGetAddressOf(), d, s);
		hr = create_ds.CopyTo(out_depth_stencil_state);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void RenderState::CreateRasterizerState(
	ID3D11Device* device, 
	D3D11_FILL_MODE fill,
	D3D11_CULL_MODE cull, 
	bool is_front_counter_clock_wise,
	ID3D11RasterizerState** out_rs)
{
	HRESULT hr{ S_OK };

	// ビットシフトを用いて一つのインデックスにする。（各データ内の数値的にオーバーフローしない）
	auto get_index = [](D3D11_FILL_MODE f, D3D11_CULL_MODE c, bool w) -> uint32_t
	{
		return (f << 9) |
			   (c << 1) |
			   (w);		 
	};

	uint32_t index = get_index(fill, cull, is_front_counter_clock_wise);

	auto found{ rasterizer_states.find(index) };

	if (found != rasterizer_states.end())
	{
		hr = found->second.CopyTo(out_rs);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		// 作成したステートは配列に登録される。
		// 新しい設定のステートを作成し、そのアドレスをコピーする。
		auto& create_rs{ rasterizer_states[index] };
		DXHelper::CreateRasterizerState(device, create_rs.ReleaseAndGetAddressOf(), fill, cull, is_front_counter_clock_wise);
		hr = create_rs.CopyTo(out_rs);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void RenderState::CreateSamplerState(
	ID3D11Device* device, 
	D3D11_FILTER filter, 
	D3D11_TEXTURE_ADDRESS_MODE address, 
	D3D11_COMPARISON_FUNC comparison, 
	ID3D11SamplerState** out_ss)
{
	HRESULT hr{ S_OK };

	// ビットシフトを用いて一つのインデックスにする。（各データ内の数値的にオーバーフローしない）
	auto get_index = [](D3D11_FILTER f, D3D11_TEXTURE_ADDRESS_MODE a, D3D11_COMPARISON_FUNC c) -> uint32_t
	{
		return (f << 16) |
			   (a << 8 ) |
			   (c);
	};

	uint32_t index = get_index(filter, address, comparison);

	auto found{ sampler_states.find(index) };

	if (found != sampler_states.end())
	{
		hr = found->second.CopyTo(out_ss);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else
	{
		// 作成したステートは配列に登録される。
		// 新しい設定のステートを作成し、そのアドレスをコピーする。
		auto& create_ss{ sampler_states[index] };
		DXHelper::CreateSamplerState(device, create_ss.ReleaseAndGetAddressOf(), filter, address, comparison);
		hr = create_ss.CopyTo(out_ss);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}
