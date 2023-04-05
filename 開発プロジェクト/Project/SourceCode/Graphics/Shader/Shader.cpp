#include "Shader.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/RenderState.h"

/* --- 関数の実装 ---- */

Shader::Shader()
{
}

Shader::~Shader()
{
}

void Shader::CreateVSFromCSO(ID3D11Device* device, const char* filepath, 
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* desc, UINT num)
{
	ResourceFileLoader::CreateVSfromCSO(device, filepath, vertex_shader.ReleaseAndGetAddressOf(), input_layout, desc, num);
}

void Shader::CreatePSFromCSO(ID3D11Device* device, const char* filepath)
{
	ResourceFileLoader::CreatePSfromCSO(device, filepath, pixel_shader.ReleaseAndGetAddressOf());
}

void Shader::CreateGSFromCSO(ID3D11Device* device, const char* filepath)
{
	ResourceFileLoader::CreateGSfromCSO(device, filepath, geometry_shader.ReleaseAndGetAddressOf());
}

void Shader::CreateHSFromCSO(ID3D11Device* device, const char* filepath)
{
	ResourceFileLoader::CreateHSfromCSO(device, filepath, hull_shader.ReleaseAndGetAddressOf());
}

void Shader::CreateDSFromCSO(ID3D11Device* device, const char* filepath)
{
	ResourceFileLoader::CreateDSfromCSO(device, filepath, domain_shader.ReleaseAndGetAddressOf());
}

void Shader::CreateBlendState(ID3D11Device* device, BSMode bs_mode)
{
	Graphics::GetInstance().GetRenderState()->CreateBlendState(device, bs_mode, blend_state.ReleaseAndGetAddressOf());
}

void Shader::CreateDepthStencilState(ID3D11Device* device, DSMode ds_mode)
{
	Graphics::GetInstance().GetRenderState()->CreateDepthStencilState(device, ds_mode, depth_stencil_state.ReleaseAndGetAddressOf());
}

void Shader::CreateRasterizerState(ID3D11Device* device, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, bool is_front_counter_clock_wise)
{
	Graphics::GetInstance().GetRenderState()->CreateRasterizerState(device, fill_mode, cull_mode, is_front_counter_clock_wise, rasterizer_state.ReleaseAndGetAddressOf());
}

void Shader::CreateSamplerState(ID3D11Device* device, UINT slot_num, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func)
{
	auto add_sampler_state{ 
		sampler_states.emplace(slot_num, Microsoft::WRL::ComPtr<ID3D11SamplerState>()) };

	Graphics::GetInstance().GetRenderState()->CreateSamplerState(device, filter, address_mode, comparison_func, add_sampler_state.first->second.ReleaseAndGetAddressOf());

#ifdef _DEBUG
	// 既に構築（同じキーにステートを生成していた）場合は、警告を出しておく
	if (!add_sampler_state.second)
	{
		OutputDebugString(L"同じキーに再度サンプラーステート構築しました。");
	}
#endif // _DEBUG
}

void Shader::Activate(ID3D11DeviceContext* dc) 
{
	dc->VSSetShader(vertex_shader.Get()		, nullptr, 0);
	dc->HSSetShader(hull_shader.Get()		, nullptr, 0);
	dc->DSSetShader(domain_shader.Get()		, nullptr, 0);
	dc->GSSetShader(geometry_shader.Get()	, nullptr, 0);
	dc->PSSetShader(pixel_shader.Get()		, nullptr, 0);

	ActivateRenderState(dc);
}

void Shader::ActivateRenderState(ID3D11DeviceContext* dc) 
{
	if(blend_state)			dc->OMSetBlendState(blend_state.Get(), nullptr, 0xffffffff);
	if(depth_stencil_state) dc->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	if(rasterizer_state)    dc->RSSetState(rasterizer_state.Get());

	// サンプラーステートの配列を作成し、バインドする。
	if(!sampler_states.empty())
	{
		// 内部的に、map コンテナは各要素をキーの値に従って下位から上位へと並べており、従って rbegin() は最上位のキーにあたる値を返す。
		size_t siz{ static_cast<size_t>(sampler_states.rbegin()->first + 1ULL) };
		std::vector<ID3D11SamplerState*> ss(siz, nullptr);

		for (const auto& s : sampler_states)
		{
			ss[s.first] = s.second.Get();
		}

		dc->VSSetSamplers(0, siz, ss.data());
		dc->HSSetSamplers(0, siz, ss.data());
		dc->DSSetSamplers(0, siz, ss.data());
		dc->GSSetSamplers(0, siz, ss.data());
		dc->PSSetSamplers(0, siz, ss.data());
	}
}

void Shader::Deactivate(ID3D11DeviceContext* dc)
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->HSSetShader(nullptr, nullptr, 0);
	dc->DSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}

