#include "EffekseerEffect.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "General/AtaiEngine.h"

#include "Graphics/General/Graphics.h"
#include "EffekseerManager.h"

/* --- ��` ---- */

void EffekseerEffect::Create(Effekseer::Manager* manager, const char* filepath)
{
	//// �G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
	//// �}���`�X���b�h��Effect���쐬�����DeviceContext�𓯎��A�N�Z�X����
	//// �t���[�Y����\��������̂Ŕr�����䂷��
	//std::lock_guard<std::mutex> lock(DXSystem.GetMutex());

	// Effekseer�̃��\�[�X��ǂݍ���
	// Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�ϊ����K�v
	char16_t utf_16_filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf_16_filename, 256, filepath);

	// Effekseer�G�t�F�N�g�ǂݍ���
	effect = Effekseer::Effect::Create(manager, (EFK_CHAR*)utf_16_filename);
}

Effekseer::Handle EffekseerEffect::Play(
	const Vector3& position)
{
	const auto& manager{ Graphics::GetInstance().GetEffekseerManager()->GetEffekseerManager() };

	auto handle = manager->Play(effect, position.x, position.y, position.z);
	manager->SetScale(handle, 1.0f, 1.0f, 1.0f);
	manager->SetRotation(handle, 0, 0, 0);

	return handle;
}
