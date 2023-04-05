#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <unordered_map>

#include "Include/misc.h"
#include "../PixelBuffer.h" // �J���[�t�H�[�}�b�g����Ή�����t�H�[�}�b�g�����肷��B

/* --- �錾 ---- */

class Texture
{
public:

	Texture();
	virtual ~Texture();

	/// <summary>
	/// �t�@�C���̃e�N�X�`����ǂݍ��ށA����p�X�̃e�N�X�`�������ɓǂݍ��܂�Ă���ꍇ�A�L���b�V���f�[�^��ǂݍ��ށB
	/// </summary>
	void Create(ID3D11Device* device, const wchar_t* filepath);
	/// <summary>
	/// �F���w�肵�ăe�N�X�`�����쐬
	/// </summary>
	void Create(ID3D11Device* device, DWORD color = 0xffffffff);
	/// <summary>
	/// �����̃r���[��ݒ肷��B�r���[���瑼�̏����擾����B
	/// </summary>
	void Create(ID3D11ShaderResourceView** src);
	/// <summary>
	/// �����̃f�[�^����e�N�X�`���A�r���[���쐬����B
	/// </summary>
	void Create(
		ID3D11Device* device, 
		uint32_t width, 
		uint32_t height, 
		DXGI_FORMAT format, 
		uint32_t bind_flag);
	/// <summary>
	/// �����̃f�[�^����e�N�X�`���A�r���[���쐬����B
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
