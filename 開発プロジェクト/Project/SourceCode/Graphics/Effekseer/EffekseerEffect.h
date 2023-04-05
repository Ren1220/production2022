#pragma once

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <Effekseer.h>
#include <optional>

#include "Arithmetic/Arithmetic.h"

/* --- �錾 ---- */

class EffekseerEffect
{
public:

	EffekseerEffect() = default;
	~EffekseerEffect()
	{
		// �j������
		if (effect != nullptr)
		{
			effect->Release();
			effect = nullptr;
		}
	}

	void Create(Effekseer::Manager* manager, const char* filepath);

	/// <summary>
	/// ���̃G�t�F�N�g��V�����������āA���������G�t�F�N�g�𑀍삷�邽�߂̃n���h����Ԃ��B
	/// </summary>
	Effekseer::Handle Play(const Vector3& position);

private:

	Effekseer::Effect* effect{ nullptr };
};