#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Texture/Texture.h"

/* --- �錾 ---- */

/// <summary>
/// ��{�I�ɂ�Graphic����ă����_�[�^�[�Q�b�g��ݒ肵�A
/// ���̍H���̒���Graphic�ɐݒ肵���������_�[�^�[�Q�b�g�̃|�C���^��ێ������邽�߁A
/// shared_ptr�Ő�������B
/// </summary>
class RenderTarget
{
public:

	RenderTarget() : render_target_view(), texture() {}
	~RenderTarget() = default;

	/// <summary>
	/// �g�p����t�H�[�}�b�g�̓V�F�[�_���\�[�X�Ƃ��ĎQ�Ƃ���Ƃ��̃t�H�[�}�b�g���w�肷��B
	/// </summary>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT color_format);
	
	/// <summary>
	/// ���ׂĎ��g�Őݒ肷��Ƃ�
	/// </summary>
	void Create(ID3D11Device* device,
				const D3D11_TEXTURE2D_DESC* tex_desc,
				const D3D11_SUBRESOURCE_DATA* subresource,
				const D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc,
				const D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc);

	void Clear(ID3D11DeviceContext* dc, const float clear_color[4] = nullptr);

	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetView() { return render_target_view; }
	const Texture& GetTexture()										{ return texture; }

private:

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Texture texture;
};
