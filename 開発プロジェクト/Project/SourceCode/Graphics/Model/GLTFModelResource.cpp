#include "GLTFModelResource.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/ChronoTimer.h"

#include "GLTFModelData.h"

/* --- ��` ---- */

std::shared_ptr<GLTFModelData> GLTFModelResource::LoadModelData(ID3D11Device* device, const char* filepath)
{
    auto&& c_itr { model_data_cache.find(filepath) };
    ChronoTimer timer;

    // �L���b�V���ɓ���p�X�̃f�[�^�����݂���ꍇ
    if (c_itr != model_data_cache.end())
    {
        // ����p�X�̃f�[�^��Ԃ�
        return c_itr->second;
    }
    // �L���b�V���ɓ���p�X�̃f�[�^�����݂��邵�Ȃ��ꍇ
    else
    {
        timer.Start();
        // �p�X�̃f�[�^��ǂݍ���
        std::shared_ptr<GLTFModelData> model_data{ std::make_shared<GLTFModelData>(device, filepath) };
        timer.Stop(true, std::format(L"[{}]�̓ǂݍ��ݎ���", std::wstring(model_data->filename.begin(), model_data->filename.end())));
        // �ǂݍ��񂾃f�[�^���L���b�V���ɓo�^����
        model_data_cache.emplace(std::make_pair(filepath, model_data));
        // �ǂݍ��񂾃f�[�^��Ԃ�
        return model_data;
    }
}