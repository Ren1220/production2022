#include "GLTFModel.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "GLTFModelResource.h"

/* --- ��` ---- */

GLTFModel::GLTFModel(ID3D11Device* device, const char* filepath)
	: world_transform(Matrix::Identity())
{
	// ���\�[�X�Ǘ��N���X��ʂ��āA���f���f�[�^��ǂݍ���
	w_model_data = GLTFModelResource::GetInstance().LoadModelData(device, filepath);
}

GLTFModel::~GLTFModel()
{
}
