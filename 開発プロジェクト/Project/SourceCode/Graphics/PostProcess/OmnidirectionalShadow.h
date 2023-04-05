#pragma once

/* --- �T�v ---- */

// �S���ʃV���h�E�p�̃e�N�X�`�����쐬����N���X�B
// �S���ʃV���h�E�Ƃ͓_�����ƂȂ���W�𒆐S�Ƃ��āA�e���̐����̕����ɑ΂��Ẵr���[�s����쐬���A
// �f�肱�񂾉e��`�悷��Ώۂ̐[�x�̏������݂��s���V���h�E�B
// �e�N�X�`���̓W�I���g���V�F�[�_�[�𗘗p���Ĉ�x�̕`�揈���ŃL���[�u�}�b�v�ɏ������ނ��Ƃ��ł���B
// ����f�B�t�@�[�h�����_�����O�𗘗p���ă��C�e�B���O�̃^�C�~���O�ŃV���h�E�C���O���s���Ă���B
// �Q�l�Fhttps://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-12-omnidirectional-shadow-mapping

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <array>

#include "Utility/Singleton.h"

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Shader/Shader.h"


/* --- �錾 ---- */

/// <summary>
/// �S���ʃV���h�E
/// ���͘Z�ʂ̐[�x���o�b�t�@�ɏ������݁A�[�x�e�N�X�`�����쐬�B
/// </summary>
class OmnidirectionalShadow
{
	struct TextureCubeArray;

public:

	OmnidirectionalShadow();
	~OmnidirectionalShadow();

	/// <summary>
	/// �[�x�������݂ɕK�v�ȃf�[�^���쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="tex_w">�[�x���������ރL���[�u�}�b�v�̈�ʓ�����̃T�C�Y</param>
	/// <param name="tex_h">�[�x���������ރL���[�u�}�b�v�̈�ʓ�����̃T�C�Y</param>
	/// <param name="max_light_count">�[�x���������ރ��C�g�̍ő吔</param>
	/// <param name="srv_format">�[�x�e�N�X�`���̃V�F�[�_�[���\�[�X�̃t�H�[�}�b�g�A���̃t�H�[�}�b�g�͂��̏�񂩂琄�������</param>
	void Create(
		ID3D11Device* device, 
		uint32_t tex_w, 
		uint32_t tex_h, 
		uint32_t max_light_count = 5U,
		DXGI_FORMAT srv_format = DXGI_FORMAT_R32_FLOAT);

	/// <summary>
	/// �������݊J�n����
	/// </summary>
	/// <param name="dc"></param>
	void Activate(ID3D11DeviceContext* dc);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�̏������A�[�x�l��FLT_MAX�ɂ���B
	/// </summary>
	void ClearRTV(ID3D11DeviceContext* dc);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�ɏ������񂾏����L���[�u�}�b�v�z��ɏ�������
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="index">�L���[�u�}�b�v�z��̃C���f�b�N�X</param>
	void WriteTextureCubeArray(ID3D11DeviceContext* dc, int32_t index);

	/// <summary>
	/// �V���h�E�������ݗp�V�F�[�_��ݒ肵�ă��f����`�悷��
	/// </summary>
	/// <param name="dc"></param>
	void RenderModel(ID3D11DeviceContext* dc);

	/// <summary>
	/// �������ݏI������
	/// </summary>
	/// <param name="dc"></param>
	void Deactivate(ID3D11DeviceContext* dc);

	void ImGuiRender();

	const std::shared_ptr<RenderTarget>& GetRenderTarget() const { return render_target; }
	const TextureCubeArray& GetShadowCastTextureCubeArray() const { return shadow_cast_texture_cube_array; }

	void SetCBuffer(ID3D11DeviceContext* dc, uint32_t slot);

	/// <summary>
	/// �V���h�E���������ނ��߂̏��ł���
	/// �����̈ʒu�𒆐S�Ƃ����e���̐����̕����ɑ΂���r���[�s��A
	/// �v���W�F�N�V�����s��A�J�����̎��_�i�����̈ʒu�j���v�Z�E�ݒ肷��B
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="light_pos"></param>
	/// <param name="near_z">�v���W�F�N�V�����s��̍ŋߋ���</param>
	/// <param name="far_z">�v���W�F�N�V�����s��̍ŉ�����</param>
	void UpdateCBuffer(
		ID3D11DeviceContext* dc,
		const Vector3& light_pos,
		float near_z,
		float far_z);

	/// <summary>
	/// �V���h�E�C���O���s���_�����̍ő吔
	/// </summary>
	uint32_t GetMaxLightCount() const { return max_light_count; }

private:

	uint32_t max_light_count; // �V���h�E�C���O���s���_�����̍ő吔
	uint32_t width, height;   // �V���h�E�}�b�v�̈�ʓ�����̃e�N�X�`���T�C�Y

	/// <summary>
	/// �V���h�E���������ރ����_�[�^�[�Q�b�g�̍쐬
	/// </summary>
	void CreateRenderTarget(ID3D11Device* device, DXGI_FORMAT srv_format);

	/// <summary>
	/// �V���h�E���������ރL���[�u�}�b�v�̍쐬
	/// </summary>
	/// <param name="max_array">�L���[�u�}�b�v�̐�</param>
	void CreateTextureCubeArray(ID3D11Device* device, uint32_t max_array, DXGI_FORMAT srv_format);
	
	/// <summary>
	/// �����̍��W�ɋ���z�u���A�[�x�����������ށB
	/// </summary>
	void RenderCircle(ID3D11DeviceContext* dc, const Vector3& position, float radius);

	std::shared_ptr<GLTFModel> circle_model;

	std::shared_ptr<RenderTarget> render_target;

	// �W�I���g���V�F�[�_�[��p���Ĉ�x�ɃL���[�u�}�b�v�Ɋe�����̐[�x����������
	Shader shadow_cast_shader; 
	Shader shadow_cast_shader_instanced; // �n�`�̓C���X�^���X�`��Ȃ̂ł���p�̃V�F�[�_�[

	struct CBufferCubeMapView
	{
		static constexpr UINT SLOT{ 3 };

		std::array<Matrix, 6> view; // �E�A���A��A���A���A��
		Matrix projection;			// �v���W�F�N�V�����s��͑S�����ŋ��ʂ̕��𗘗p�ł���
		Vector4 eye;
	};
	ConstantBuffer<CBufferCubeMapView> cube_map_view_cb;
	
	struct TextureCubeArray
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	} shadow_cast_texture_cube_array;


#ifdef _DEBUG
	bool is_shadow_cast;
	// ImGui�ŃL���[�u�}�b�v�̃e�N�X�`�����Q�Ƃ��邽�߂̃o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D11Texture2D>, 6>			copy_textures;
	std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 6>	copy_srvs;
#endif // _DEBUG
};
