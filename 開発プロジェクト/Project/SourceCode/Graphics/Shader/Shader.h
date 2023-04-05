#pragma once

/* --- �w�b�_�[�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <map> // �T���v���[�X�e�[�g��ێ�����z��\�[�g���邽�߂�����

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/Resource/RenderState.h"

/* --- �錾 ---- */

/// <summary>
/// �V�F�[�_�[(VS,HS,DS,GS,PS)�A�`��X�e�[�g(Blend,Rasterizer,DepthStencil,Sampler)��ݒ肷��B
/// �`��X�e�[�g�Ɋւ��ẮA���\�[�X�̖��ʂ�����邽�߁A�d�������ݒ�̃X�e�[�g�͑��̃V�F�[�_�[�N���X�Ƌ��L�����B
/// </summary>
class Shader
{
public:

	Shader();
	virtual ~Shader();

	virtual void Create(ID3D11Device* device) {};

	/// <summary>
	/// ���_�V�F�[�_�[�̍쐬�B�Ή�����InputLayout���Ȃ��ꍇ�͂����ō쐬����K�v������B
	/// </summary>
	/// <param name="filepath">�g���q�܂�</param>
	void CreateVSFromCSO(ID3D11Device* device, const char* filepath, 
						 ID3D11InputLayout** input_layout = nullptr, D3D11_INPUT_ELEMENT_DESC* desc = nullptr, UINT num = NULL);

	/// <summary>
	/// �n���V�F�[�_�[�̍쐬
	/// </summary>
	void CreateHSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// �h���C���V�F�[�_�[�̍쐬
	/// </summary>
	void CreateDSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// �W�I���g���V�F�[�_�[�̍쐬
	/// </summary>
	void CreateGSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// �s�N�Z���V�F�[�_�[�̍쐬
	/// </summary>
	void CreatePSFromCSO(ID3D11Device* device, const char* filepath);

	/// <summary>
	/// �u�����h�X�e�[�g�̍쐬
	/// </summary>
	void CreateBlendState(ID3D11Device* device, BSMode bs_mode);

	/// <summary>
	/// �[�x�X�e���V���X�e�[�g�̍쐬
	/// </summary>
	void CreateDepthStencilState(ID3D11Device* device, DSMode ds_mode);

	/// <summary>
	/// ���X�^���C�U�X�e�[�g�̍쐬
	/// </summary>
	void CreateRasterizerState(
		ID3D11Device* device, 
		D3D11_FILL_MODE fill_mode, 
		D3D11_CULL_MODE cull_mode = D3D11_CULL_BACK,			
		bool is_front_counter_clock_wise = false);  

	/// <summary>
	/// �T���v���[�X�e�[�g�̍쐬�B���̕`��X�e�[�g�ƈقȂ�A�V�F�[�_�[�Ƀo�C���h����ۂ̃C���f�b�N�X��key�Ƃ���map�z��ɐ��������X�e�[�g��}������B
	/// </summary>
	/// <param name="slot_num">�V�F�[�_�[�Ŏg�p����ۂ̔ԍ�</param>
	void CreateSamplerState(
		ID3D11Device* device,
		UINT slot_num,
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER);

	/// <summary>
	/// �V�F�[�_�[���f�o�C�X�R���e�L�X�g�ɐݒ肷��B
	/// �����ɕ`��X�e�[�g�̃o�C���h���s���B
	/// </summary>
	virtual void Activate(ID3D11DeviceContext* dc) ;

	/// <summary>
	/// �ݒ肵���V�F�[�_�[�ƕ`��X�e�[�g����������B
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