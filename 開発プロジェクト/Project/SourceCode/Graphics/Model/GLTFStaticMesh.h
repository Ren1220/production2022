#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "GLTFModel.h"

#include "../Resource/ConstantBuffer.h"

/* --- �錾 ---- */

class GLTFStaticMesh : public GLTFModel
{
public:

	GLTFStaticMesh(ID3D11Device* device, const char* filepath);
	virtual ~GLTFStaticMesh();

	/// <param name="shader">�V�F�[�_��ݒ肵�Ȃ��ꍇ�͔ėp�V�F�[�_�[�𗘗p����</param>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const override;
	/// <summary>
	/// �C���X�^���X�`��ł́A���f���f�[�^�̃}�e���A���\���̃o�b�t�@�͋@�\���Ȃ�
	/// �i���b�V���ɂ���ăe�N�X�`�������݂��Ȃ����̂�����j���߁A�e���b�V���̏�񂩂�r���[��
	/// �쐬����K�v������B���A���󕡐��̃��b�V���������f���̃C���X�^���X�`��͖������B
	/// TODO : �C���X�^���X�`��p���f�������B
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="material_structured_buffer_view"></param>
	void Render(ID3D11DeviceContext* dc, Shader* shader, ID3D11ShaderResourceView* material_structured_buffer_view) const ;

private:

	/// <summary>
	/// �m�[�h�C���f�b�N�X�̃��b�V����`�悷��ċA�֐�
	/// </summary>
	void DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const;

	struct CBufferPrimitive
	{
		static constexpr uint32_t slot{ 0 };

		Matrix world;
		int material;
		int has_tangent;
		int skin{ -1 };
		int pad;
	};

	ConstantBuffer<CBufferPrimitive> primitive_cb;
};
