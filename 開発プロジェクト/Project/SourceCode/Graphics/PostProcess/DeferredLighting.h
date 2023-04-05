#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>
#include <functional>

#include "../Resource/ConstantBuffer.h"
#include "../Shader/Shader.h"

#include "GeometryBuffer.h"
#include "OmnidirectionalShadow.h"

/* --- �錾 ---- */


class DeferredLighting
{
public:

	DeferredLighting()
		: width(), height() {}
	~DeferredLighting() = default;

	/// <summary>
	/// <para>�_�����A���s��</para>
	/// <para>light_parameter��w�̒l�ɂ���Č����̎�ށA�p�����[�^�̓��e�����肷��</para>
	/// <para>w = 0.0�̂Ƃ� param = { ���̌���xyz  , 0.0 }</para>
	/// <para>w > 0.0�̂Ƃ� param = { �����̈ʒuxyz, �������� }</para>
	/// <para></para>
	/// </summary>
	struct Light
	{
		Vector4 light_parameter;
		Vector3 light_color; // �F
		float luminance;	 // ���邳
	};

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	void EndProcess(ID3D11DeviceContext* dc);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �W�I���g���o�b�t�@�ɏ������ރ����_�[�^�[�Q�b�g�r���[��ݒ肷��
	/// </summary>
	void ActivateGeometryBuffer(ID3D11DeviceContext* dc);

	/// <summary>
	/// Effekseer�̃G�t�F�N�g���W�I���g���o�b�t�@��emissive�ɏ������ނ��߂̏���
	/// �����_�[�^�[�Q�b�g�̂O�Ԃ�emissive��ݒ肷��B
	/// �[�x�X�e���V�����B
	/// </summary>
	/// <param name="dc"></param>
	void ActivateBufferForEffekseer(ID3D11DeviceContext* dc);

	/// <summary>
	/// �S���ʃV���h�E�}�b�v�ɃA�N�^�A�n�`�̐[�x�����݂��s���B
	/// �V���h�E�C���O���s���̂͗\�ߓo�^���ꂽ�_�����̂�
	/// </summary>
	/// <param name="dc"></param>
	void ShadowCast(
		ID3D11DeviceContext* dc);

	/// <summary>
	/// �o�b�t�@�Ƀ��C�e�B���O���ʂ���������
	/// </summary>
	/// <param name="dc"></param>
	void Lighting(ID3D11DeviceContext* dc);

	const Texture& GetTexture() { return lighting_rt->GetTexture(); }

	void ImGuiRender();
	/// <summary>
	/// ��������o�^����
	/// (�|�C���^����Q�Ƃ���)
	/// </summary>
	/// <param name="light_data"></param>
	void RegisterLight(const std::shared_ptr<Light>& light_data);

private:

	uint32_t width, height;

	// �������̎�Q��
	std::vector<std::weak_ptr<Light>> w_lights;

	GeometryBuffer geometry_buffer;
	std::shared_ptr<RenderTarget> lighting_rt;

	// G-Buffer�p�X����擾������񂩂烉�C�e�B���O���ʕ`�悷��
	Shader lighting_shader; 

	/// <summary>
	/// ���C�e�B���O�ɗ��p��������̏��萔�o�b�t�@
	/// </summary>
	struct CBufferLight
	{
		static constexpr uint32_t SLOT{ 3 };

		Light light[10];
	};
	ConstantBuffer<CBufferLight> light_cb;

	// �_�����̑S���ʃV���h�E���s���N���X
	OmnidirectionalShadow shadow_caster;

};
