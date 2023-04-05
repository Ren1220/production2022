#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/Texture/Texture.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Shader/Shader.h"

/* --- �O���錾 ---- */



/* --- �錾 ---- */

class ColorFilter
{
public:

	ColorFilter();
	~ColorFilter();

	struct CBufferFilter
	{
		static constexpr UINT SLOT{ 3U };

		float hue_shift;  // �F��
		float saturation; // �ʓx
		float brightness; // ���x
		float contrast;   // �R���g���X�g
	};

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	/// <summary>
	/// �����̃e�N�X�`���ɃJ���[�t�B���^�[��K�p����
	/// </summary>
	void Apply(ID3D11DeviceContext* dc, const Texture& texture);

	/// <summary>
	/// �t�B���^�[���������e�N�X�`�����擾
	/// </summary>
	/// <returns></returns>
	const Texture& GetTexture() const { return filter_rt->GetTexture(); }

	const CBufferFilter& GetFilterParam() const { return filter_param; }
	void SetFilterParam(const CBufferFilter& p) { filter_param = p; }

private:

	Shader filter_shader;

	CBufferFilter filter_param;
	ConstantBuffer<CBufferFilter> filter_cb;

	std::shared_ptr<RenderTarget> filter_rt;

};
