#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>

#include "../Shader/Shader.h"
#include "../Resource/RenderTarget.h"

#include "GaussianBlur.h"

/* --- �錾 ---- */

/// <summary>
/// �쐣���u���[��
/// </summary>
class Bloom
{
public:

	Bloom() 
		: width(), height() {};
	~Bloom() = default;

	void Initialize(
		ID3D11Device* device, 
		uint32_t width, 
		uint32_t height, 
		DXGI_FORMAT format);

	/// <summary>
	/// �����̃e�N�X�`���̋P�x�𒊏o���A�K�E�X�u���[���_�E���T���v�����O�Ƌ��ɕ�����s���A�u���[���e�N�X�`�����쐬����.
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="scene_texture"></param>
	void Apply(ID3D11DeviceContext* dc, const Texture& scene_texture);

	/// <summary>
	/// �u���[���e�N�X�`�����擾����B
	/// </summary>
	/// <returns></returns>
	const Texture& GetTexture() const { return blur_composition_render_target->GetTexture(); }

private:

	uint32_t width, height;

	/// <summary>
	/// �P�x�𒊏o����
	/// </summary>
	/// <param name="dc">�f�o�C�X�R���e�L�X�g</param>
	/// <param name="texture">�P�x�𒊏o����e�N�X�`����</param>
	void SamplingLuminance(ID3D11DeviceContext* dc, const Texture& scene_texture);
	/// <summary>
	/// �P�x�𒊏o�����e�N�X�`���Ƀu���[��������
	/// </summary>
	/// <param name="dc">�f�o�C�X�R���e�L�X�g</param>
	virtual void ApplyBlurEffect(ID3D11DeviceContext* dc);
	/// <summary>
	/// �u���[�����������ׂĂ̋P�x�e�N�X�`������������B
	/// </summary>
	/// <param name="dc"></param>
	void CompositionBlur(ID3D11DeviceContext* dc);

	std::unique_ptr<Shader> luminance_shader;			   // �P�x���o
	std::shared_ptr<RenderTarget> luminance_render_target; // �P�x���o

	static constexpr uint32_t blur_sample_num{ 4 };

	std::array<GaussianBlur, blur_sample_num> blur_samples;

	std::shared_ptr<RenderTarget> blur_composition_render_target;
	std::unique_ptr<Shader> blur_composition_shader;

};
