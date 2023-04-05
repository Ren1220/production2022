#pragma once

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <string>
#include <sstream>
#include <unordered_map>
#include <d3d11.h>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "Include/misc.h"

/* --- �錾 ---- */

class ResourceFileLoader
{
public:

	/// <summary>
	/// �e�N�X�`���̓ǂݍ���
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath">�t�@�C���p�X</param>
	/// <param name="shader_resource_view">�V�F�[�_���\�[�X�r���[</param>
	/// <param name="texture2d_desc">�e�N�X�`�����</param>
	/// <returns>��肪�Ȃ��ꍇ "S_OK" ��Ԃ�</returns>
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
	/// �_�~�[�e�N�X�`���̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="value">�s�N�Z���ɓ��͂���F �J���[�R�[�h(#0xAABBGGRR)</param>
	/// <param name="dimension">���s�N�Z���ŋ�؂邩</param>
	/// <returns>��肪�Ȃ��ꍇ "S_OK"	 ��Ԃ�</returns>
	static HRESULT CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view,
		DWORD value /*0xAABBGGRR*/, UINT dimension);

	/// <summary>
	/// �_�~�[�e�N�X�`���̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="value">�s�N�Z���ɓ��͂���F �J���[�R�[�h(#0xAABBGGRR)</param>
	/// <param name="dimension">���s�N�Z���ŋ�؂邩</param>
	/// <returns>��肪�Ȃ��ꍇ "S_OK"	 ��Ԃ�</returns>
	static HRESULT CreateDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** out_shader_resource_view, ID3D11Texture2D** out_texture2d,
		DWORD value /*0xAABBGGRR*/, UINT dimension);

	/// <summary>
	/// ���_�V�F�[�_�[�̓ǂݍ���
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="cso_name">�ǂݍ��ޒ��_�V�F�[�_�[�t�@�C��</param>
	/// <param name="vertex_shader">���_�V�F�[�_�[</param>
	/// <param name="input_layout">�C���v�b�g���C�A�E�g</param>
	/// <param name="input_element_desc"></param>
	/// <param name="num_elements">�C���v�b�g���C�A�E�g�f�X�N�̔z��</param>
	/// <returns></returns>
	static HRESULT CreateVSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	/// <summary>
	/// �s�N�Z���V�F�[�_�[�̓ǂݍ���
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="cso_name">�ǂݍ��݃s�N�Z���V�F�[�_�[�t�@�C��</param>
	/// <param name="pixel_shader">�s�N�Z���V�F�[�_�[</param>
	/// <returns></returns>
	static HRESULT CreatePSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	/// <summary>
	/// �W�I���g���V�F�[�_�[�̓ǂݍ���
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="cso_name">�ǂݍ��݃W�I���g���V�F�[�_�[�t�@�C��</param>
	/// <param name="geometry_shader">�W�I���g���V�F�[�_�[</param>
	/// <returns></returns>
	static HRESULT CreateGSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

	static HRESULT CreateHSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);
	static HRESULT CreateDSfromCSO(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);


};