#include "ColorFilter.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

ColorFilter::ColorFilter()
	: filter_shader(), filter_param()
{
}

ColorFilter::~ColorFilter()
{
}

void ColorFilter::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	// �V�F�[�_�[�̍쐬
	filter_shader.CreateBlendState(device, BSMode::Alpha);
	filter_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso");
	filter_shader.CreatePSFromCSO(device, "Shader/ColorFilterPS.cso");
	filter_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	// �萔�o�b�t�@�̍쐬
	filter_cb.Create(device);

	filter_param.hue_shift = 0.0f;
	filter_param.saturation = 1.1f;
	filter_param.brightness = 1.0f;
	filter_param.contrast = 1.5f;

	// �����_�[�^�[�Q�b�g�̍쐬
	filter_rt = std::make_shared<RenderTarget>();
	filter_rt->Create(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void ColorFilter::Apply(ID3D11DeviceContext* dc, const Texture& texture)
{
	// �萔�o�b�t�@�̍X�V
	filter_cb.UpdateSubresouce(dc, filter_param);

	// �萔�o�b�t�@�̐ݒ�
	Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(
		dc, CBufferFilter::SLOT, &filter_cb);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	float width{ static_cast<float>(texture.GetDesc().Width) };
	float height{ static_cast<float>(texture.GetDesc().Height) };

	Graphics::GetInstance().GetFrameBuffer()->Set(
		dc,
		width,
		height,
		nullptr,
		0U,
		filter_rt);

	// �`�揈��
	Graphics::GetInstance().GetTextureRenderer()->BeginDraw(dc, &filter_shader);
	Graphics::GetInstance().GetTextureRenderer()->Draw(dc, texture);
	Graphics::GetInstance().GetTextureRenderer()->EndDraw(dc);
}
