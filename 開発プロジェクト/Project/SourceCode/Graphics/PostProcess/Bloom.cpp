#include "Bloom.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

void Bloom::Initialize(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;


	// �P�x���o�o�b�t�@
	{
		// �����_�[�^�[�Q�b�g
		luminance_render_target = std::make_shared<RenderTarget>();
		luminance_render_target->Create(device, width, height, format);

		// �V�F�[�_�[
		luminance_shader = std::make_unique<Shader>();
		luminance_shader->CreateVSFromCSO(device, "Shader/LuminanceSamplingVS.cso");
		luminance_shader->CreatePSFromCSO(device, "Shader/LuminanceSamplingPS.cso");

		luminance_shader->CreateBlendState(device, BSMode::Add);
	}
	
	// �u���[����
	{
		// �����_�[�^�[�Q�b�g
		blur_composition_render_target = std::make_shared<RenderTarget>();
		blur_composition_render_target->Create(device, width, height, format);

		// �V�F�[�_�[
		blur_composition_shader = std::make_unique<Shader>();
		blur_composition_shader->CreateVSFromCSO(device, "Shader/BlurCompositionVS.cso");
		blur_composition_shader->CreatePSFromCSO(device, "Shader/BlurCompositionPS.cso");

		blur_composition_shader->CreateBlendState(device, BSMode::Add);
	}

	// �K�E�X�u���[
	// �u���[���������e�N�X�`���ɍĂуu���[��������B
	// �����ăe�N�X�`���������Ĉ������΂��ĕ`�悷�邱�ƂŁA
	// ��������s�N�Z���̐������炷
	for (size_t bi = 0; bi < blur_samples.size(); ++bi)
	{
		uint32_t w{ width  / (1 << bi) }; // width  / 2^bi
		uint32_t h{ height / (1 << bi) }; // height / 2^bi

		blur_samples[bi].Initialize(device, w, h, format);
	}
}

void Bloom::Apply(ID3D11DeviceContext* dc, const Texture& scene_texture)
{
	// �e�N�X�`���̋P�x�𒊏o����
	SamplingLuminance(dc, scene_texture);

	// ���o�����e�N�X�`���ɕ�����u���[��������
	ApplyBlurEffect(dc);

	// �쐬�����u���[�e�N�X�`������������
	CompositionBlur(dc);
}

void Bloom::SamplingLuminance(ID3D11DeviceContext* dc, const Texture& scene_texture)
{
	auto& graphics{ Graphics::GetInstance() };
	auto& frame_buffer{ graphics.GetFrameBuffer()};
	auto& texture_renderer{ graphics.GetTextureRenderer() };

	// �����_�[�^�[�Q�b�g��ύX����
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		luminance_render_target);

	texture_renderer->BeginDraw(dc, luminance_shader.get());
	// �`�揈�����s���P�x�𒊏o�����e�N�X�`�����쐬����
	texture_renderer->Draw(dc, scene_texture);
}

void Bloom::ApplyBlurEffect(ID3D11DeviceContext* dc)
{
	// �P�x�𒊏o�����e�N�X�`���ɕ�����u���[��������
	{
		blur_samples[0].Apply(dc, luminance_render_target->GetTexture());

		for (uint32_t bi = 1U; bi < blur_sample_num; bi++)
		{
			// �u���[���������e�N�X�`���ɍĂуu���[��������B
			// �����ăe�N�X�`���������Ĉ������΂��ĕ`�悷�邱�ƂŁA
			// ��������s�N�Z���̐������炷
			blur_samples[bi].Apply(dc, blur_samples[static_cast<size_t>(bi) - 1ULL].GetTexture());
		}
	}
}

void Bloom::CompositionBlur(ID3D11DeviceContext* dc)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer()};
	auto& graphics{ Graphics::GetInstance() };

	// �����_�[�^�[�Q�b�g��ύX����
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		blur_composition_render_target);

	std::array<ID3D11ShaderResourceView*, blur_sample_num> srvs{};
	for (uint32_t bi = 0; bi < blur_sample_num; ++bi)
	{
		srvs[bi] = blur_samples[bi].GetTexture().GetView().Get();
	}

	graphics.GetTextureRenderer()->BeginDraw(dc, blur_composition_shader.get());
	// �`�揈�����s���P�x�𒊏o�����e�N�X�`�����쐬����
	graphics.GetTextureRenderer()->Draw(
		dc, 
		srvs.data(),
		static_cast<uint32_t>(srvs.size()), 
		static_cast<float>(width),
		static_cast<float>(height));
}

