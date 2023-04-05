#include "GaussianBlur.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

GaussianBlur::GaussianBlur()
	: width(), height(), weights()
{
}

void GaussianBlur::Initialize(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;

	blur_x_render_target = std::make_shared<RenderTarget>();
	blur_x_render_target->Create(device, width, height, format);
	blur_y_render_target = std::make_shared<RenderTarget>();
	blur_y_render_target->Create(device, width, height, format);

	{
		blur_x_shader = std::make_unique<Shader>();
		blur_y_shader = std::make_unique<Shader>();

		blur_x_shader->CreateVSFromCSO(device, "Shader/GausBlurXVS.cso");
		blur_x_shader->CreatePSFromCSO(device, "Shader/GausBlurPS.cso");
		blur_y_shader->CreateVSFromCSO(device, "Shader/GausBlurYVS.cso");
		blur_y_shader->CreatePSFromCSO(device, "Shader/GausBlurPS.cso");

		// �A�h���X�{�[�_�[�A���j�A���
		blur_x_shader->CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
		blur_y_shader->CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
	}

	// �u���[�̏d�݃e�[�u��
	weight_cb.Create(device);
}

void GaussianBlur::CalcWeightTable(float distribution)
{
	// �d�݂̍��v���L�^����ϐ����`����
	float total = 0;

	// ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���
	// ���[�v�ϐ���x����e�N�Z������̋���
	for (int x = 0; x < num_weights; x++)
	{
		weights[x] = expf(-0.5f * (float)(x * x) / distribution);
		total += 2.0f * weights[x];
	}

	// �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���
	for (int i = 0; i < num_weights; i++)
	{
		weights[i] /= total;
	}
}

void GaussianBlur::Apply(
	ID3D11DeviceContext* dc, 
	const Texture& texture, 
	float distribution)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer()};
	auto& graphics{ Graphics::GetInstance() };

	// �萔�o�b�t�@�̍X�V
	{
		CBufferWeight cb;
		CalcWeightTable(distribution);
		memcpy_s(&cb, sizeof(CBufferWeight), weights.data(), sizeof(float) * num_weights);
		weight_cb.UpdateSubresouce(dc, cb);

		// �萔�o�b�t�@��ݒ�
		graphics.GetConstantBufferManager()->SetBuffer(dc, CBufferWeight::slot, &weight_cb);
	}

	// ���u���[��������
	{
		// �����_�[�^�[�Q�b�g�̐ݒ�
		frame_buffer->Set(
			dc,
			static_cast<float>(width),
			static_cast<float>(height),
			nullptr,
			0,
			blur_x_render_target);

		blur_x_shader->Activate(dc);
		graphics.GetTextureRenderer()->Draw(dc, width, height, texture);
	}

	// ���u���[���������e�N�X�`���ɏc�u���[��������
	{
		// �����_�[�^�[�Q�b�g�̐ݒ�
		frame_buffer->Set(
			dc,
			static_cast<float>(width),
			static_cast<float>(height),
			nullptr,
			0,
			blur_y_render_target);

		blur_y_shader->Activate(dc);
		graphics.GetTextureRenderer()->Draw(dc, width, height, blur_x_render_target->GetTexture());
	}
}