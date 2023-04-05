#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <array>
#include <memory>

#include "Arithmetic/Arithmetic.h"

#include "../Shader/Shader.h"

#include "GLTFModelData.h"

/* --- �錾 ---- */

class GLTFModel
{
public:

	GLTFModel(ID3D11Device* device, const char* filepath);
	virtual ~GLTFModel();

	/// <summary>
	/// ���[���h�s���C�ӂ̍��W�n�ɕϊ����Đݒ肷��
	/// </summary>
	void SetWorldTransform(const Matrix& world_transform, const Matrix& coordinate_space = Matrix::RhsYup())
	{
		this->world_transform = coordinate_space * world_transform;
	}

	const Matrix& GetWorldTransform() const { return world_transform; }
	std::shared_ptr<GLTFModelData> GetModelData() const { return w_model_data.lock(); }

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="shader">�V�F�[�_��ݒ肵�Ȃ��ꍇ�͔ėp�V�F�[�_�[�𗘗p����</param>
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const = 0;

private:

	std::weak_ptr<GLTFModelData> w_model_data; 

	Matrix world_transform; 

};

