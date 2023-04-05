#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <array>

#include "../Resource/ConstantBuffer.h"
#include "../Resource/RenderTarget.h"
#include "../Shader/Shader.h"

/* --- �錾 ---- */

class GaussianBlur
{
public:

	GaussianBlur();

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format);
	/// <summary>
	/// �����̃e�N�X�`���ɃK�E�X�u���[���������e�N�X�`�������g�̃����_�[�^�[�Q�b�g�e�N�X�`���ɏ������ށB
	/// �������񂾃e�N�X�`���� GetTexture() �Ŏ擾�B
	/// </summary>
	/// <param name="texture">���̃e�N�X�`����������������킯�ł͂Ȃ�</param>
	/// <param name="distribution">�{�P�</param>
	void Apply(ID3D11DeviceContext* dc, const Texture& texture, float distribution = 8.0f);

	const Texture& GetTexture() { return blur_y_render_target->GetTexture(); }

private:

	// �u���[�̏d�݃e�[�u�����v�Z����
	void CalcWeightTable(float distribution);

private:

	uint32_t width, height;

	static constexpr int num_weights{ 8 }; // 16byte�A���C�����g�P�ʂ�


	std::array<float, num_weights> weights;

	std::shared_ptr<RenderTarget> blur_x_render_target;
	std::shared_ptr<RenderTarget> blur_y_render_target;

	std::unique_ptr<Shader> blur_x_shader;
	std::unique_ptr<Shader> blur_y_shader;

	struct CBufferWeight
	{
		static constexpr uint32_t slot{ 5 };
		std::array<float, num_weights> weights; // 16byte�A���C�����g�P�ʂ�
	};
	ConstantBuffer<CBufferWeight> weight_cb;
};
