#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Texture/Texture.h"

/* --- �錾 ---- */

/// <summary>
/// ��{�I�ɂ�Graphic����ăf�v�X�X�e���V����ݒ肵�A
/// ���̍H���̒���Graphic�ɐݒ肵�����f�v�X�X�e���V���̃|�C���^��ێ������邽�߁A
/// shared_ptr�Ő�������B
/// </summary>
class DepthStencil
{
public:

	DepthStencil() : depth_stencil_view(), texture() {}
	~DepthStencil() = default;

	/// <summary>
	/// �g�p����t�H�[�}�b�g�̓V�F�[�_���\�[�X�Ƃ��ĎQ�Ƃ���Ƃ��̃t�H�[�}�b�g���w�肷��B
	/// �����ł��̃t�H�[�}�b�g�ɑΉ������[�x�X�e���V���r���[�̃t�H�[�}�b�g���g�p����B
	/// </summary>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT color_format);

	/// <summary>
	/// ���ׂĎ��g�Őݒ肷��Ƃ�
	/// </summary>
	void Create(ID3D11Device* device,
		const D3D11_TEXTURE2D_DESC* tex_desc,
		const D3D11_SUBRESOURCE_DATA* subresource,
		const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc,
		const D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc);

	void Clear(ID3D11DeviceContext* dc, uint32_t clear_flag, float depth = 1.0f, uint8_t stencil = 0);

	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetView() const { return depth_stencil_view; }
	const Texture& GetTexture()									    const { return texture; }

private:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Texture texture;
};
