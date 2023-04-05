#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "Arithmetic/Arithmetic.h"

#include "../Resource/RenderTarget.h"
#include "../Resource/DepthStencil.h"

/* --- �錾 ---- */

class GeometryBuffer
{
public:

	GeometryBuffer();
	~GeometryBuffer() = default;

	/// <summary>
	/// �o�b�t�@�̍쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="width">�e�N�X�`���̃T�C�Y</param>
	/// <param name="height">�e�N�X�`���̃T�C�Y</param>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height);
	/// <summary>
	/// �e�o�b�t�@�̃����_�[�^�[�Q�b�g�A�[�x�X�e���V����ݒ肵�āAG�o�b�t�@�ւ̏����݂��J�n����B
	/// �����_�[�^�[�Q�b�g�̏��Ԃ́A�N���X�̕ϐ��̏���
	/// </summary>
	/// <param name="dc"></param>
	void ActivateGeometry(ID3D11DeviceContext* dc);

	void ImGuiRender();

public:

	uint32_t width, height;

	std::shared_ptr<DepthStencil> depth_stencil;

	std::shared_ptr<RenderTarget> color_rt;	 			 // �F�o�b�t�@
	std::shared_ptr<RenderTarget> roughness_metallic_rt; // ���t�l�X�E���^���b�N�o�b�t�@
	std::shared_ptr<RenderTarget> emmisive_rt;	 		 // ���Ȕ����o�b�t�@
	std::shared_ptr<RenderTarget> occlusion_rt; 		 // �A�e�o�b�t�@
	std::shared_ptr<RenderTarget> normal_rt;	 		 // �@���o�b�t�@
	std::shared_ptr<RenderTarget> world_position_rt;	 // 3D�ʒu�o�b�t�@
	std::shared_ptr<RenderTarget> depth_rt;	 			 // �[�x�o�b�t�@
};
