#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "Utility/Singleton.h"

/* --- �O���錾 ---- */

class GLTFModelData;

/* --- �錾 ---- */

/// <summary>
/// �V���O���g��GLTF���f���f�[�^�Ǘ��N���X
/// </summary>
class GLTFModelResource : public Singleton<GLTFModelResource>
{
public:

	/// <summary>
	/// �L���b�V���f�[�^���������
	/// </summary>
	void ReleaseCache()
	{
		model_data_cache.clear();
	};
	/// <summary>
	/// <para>���f���f�[�^��ǂݍ���</para>
	/// <para>�L���b�V���ɓ���̃p�X�����݂��Ă���ꍇ����̃f�[�^�̎Q�Ƃ�n��</para>
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath"></param>
	/// <returns></returns>
	std::shared_ptr<GLTFModelData> LoadModelData(ID3D11Device* device, const char* filepath);

private:

	std::unordered_map<std::string, std::shared_ptr<GLTFModelData>> model_data_cache; // ���f���f�[�^�L���b�V��
};
