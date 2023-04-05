#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <tuple>


/* --- �錾 ---- */

enum class ShaderType
{
	Vertex,		// ���_�V�F�[�_�[
	Pixel,		// �s�N�Z���V�F�[�_�[
	Geometry,
	Domain,
	Hull,

};

enum class BSMode : uint8_t
{
	None,
	Alpha,		// �A���t�@����
	Add,		// ���Z����
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
/// DirectX11����w���p�[�֐�
/// </summary>
class DXHelper
{
public:
	/// <summary>
	/// �[�x�X�e���V���X�e�[�g�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="depth_stencil_state"></param>
	/// <param name="depth_enable">�[�x�e�X�g���s����</param>
	/// <param name="stencil_enable">�X�e���V���e�X�g���s����</param>
	static void CreateDepthStencilState(ID3D11Device* device, ID3D11DepthStencilState** depth_stencil_state, bool depth_enable, bool stencil_enable);
	/// <summary>
	/// �T���v���[�X�e�[�g�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sampler_state"></param>
	/// <param name="filter"></param>
	/// <param name="address_mode">�A�h���b�V���O���[�h</param>
	/// <param name="comparison_func"></param>
	static void CreateSamplerState(ID3D11Device* device, ID3D11SamplerState** sampler_state, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER);
	/// <summary>
	/// ���X�^���C�U�X�e�[�g�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="rasterizer_state"></param>
	/// <param name="fill_mode"></param>
	/// <param name="cull_mode">�`�悵�Ȃ���</param>
	/// <param name="is_front_counter_clock_wise">�|���S���̒��_�̏��Ԃ��t���ɂ��邩</param>
	static void CreateRasterizerState(ID3D11Device* device, ID3D11RasterizerState** rasterizer_state, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, bool is_front_counter_clock_wise = FALSE);
	/// <summary>
	///  �u�����h�X�e�[�g�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="blend_state"></param>
	/// <param name="blend_mode"></param>
	static void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blend_state, BSMode blend_mode);
	/// <summary>
	/// �e�X�e�[�g�̐ݒ�
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="blend_state"></param>
	/// <param name="depth_stencil_state"></param>
	/// <param name="rasterizer_state"></param>
	static void SetState(ID3D11DeviceContext* dc, ID3D11BlendState* blend_state, ID3D11DepthStencilState* depth_stencil_state, ID3D11RasterizerState* rasterizer_state);
	/// <summary>
	/// �萔�o�b�t�@�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="constant_buffer"></param>
	/// <param name="buffer_size">16�o�C�g�A���C�����g</param>
	static void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** constant_buffer, UINT buffer_size);
	/// <summary>
	/// �萔�o�b�t�@�̐ݒ�
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
	/// �v���~�e�B�u�g�|���W�[�̐ݒ�
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="before_primitive_topology">�ݒ肷��O�̃v���~�e�B�u�g�|���W�[</param>
	/// <param name="after_primitive_topology">�ݒ肷��v���~�e�B�u�g�|���W�[</param>
	static void SetPrimitiveTopology(ID3D11DeviceContext* dc, D3D11_PRIMITIVE_TOPOLOGY* before_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY after_primitive_topology);
	/// <summary>
	/// �ݒ肵�Ă���u�����h�X�e�[�g�̐ݒ���m�ۂ���
	/// </summary>
	/// <param name="dc">�f�o�C�X�R���e�L�X�g</param>
	/// <returns></returns>
	static std::tuple<Microsoft::WRL::ComPtr<ID3D11BlendState>, FLOAT*, UINT> CacheBlendState(ID3D11DeviceContext* dc);
	/// <summary>
	/// �����_�[�^�[�Q�b�g�ŉf�����e�N�X�`�����V�F�[�_�[���\�[�X�r���[�ŎQ�Ƃ���悤�Ƀo�b�t�@���쐬����
	/// </summary>
	/// <param name="device"></param>
	/// <param name="screen_width"></param>
	/// <param name="screen_height"></param>
	/// <param name="format">�J���[�o�b�t�@�̃t�H�[�}�b�g</param>
	/// <param name="out_texture2d">�쐬�����e�N�X�`��</param>
	/// <param name="out_render_target_view">�쐬���������_�[�^�[�Q�b�g�r���[</param>
	/// <param name="out_shader_resource_view">�쐬�����V�F�[�_�[���\�[�X�r���[</param>
	static void CreateRenderTargetView(ID3D11Device* device, UINT screen_width, UINT screen_height, DXGI_FORMAT format, ID3D11Texture2D** out_texture2d, ID3D11RenderTargetView** out_render_target_view, ID3D11ShaderResourceView** out_shader_resource_view);
	/// <summary>
	/// �[�x�X�e���V���r���[�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="screen_width"></param>
	/// <param name="screen_height"></param>
	static void CreateDepthStencilView(ID3D11Device* device, UINT screen_width, UINT screen_height, ID3D11DepthStencilView** out_depth_stencil_view);

};

