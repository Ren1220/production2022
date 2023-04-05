#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <Effekseer.h>

#include "Arithmetic/Arithmetic.h"

#include "EffekseerManager.h"

/* --- �錾 ---- */

/// <summary>
/// Effekseer�̃G�t�F�N�g���Đ�����ƕԂ����n���h���B
/// ���̃n���h�����g���Đ��������G�t�F�N�g�𑀍삷��B
/// </summary>
class EffekseerHandle
{
public:

	EffekseerHandle() = default;
	/// <summary>
	/// �G�t�F�N�g�̃p�X��"Data/Effect/"�ɑ����t�@�C���p�X
	/// </summary>
	EffekseerHandle(
		const char* filepath, 
		const Vector3& position);

	~EffekseerHandle();

	void Initialize(
		const char* filepath,
		const Vector3& position);

	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale);
	void SetTransform(const Matrix& transform);

	void SetPosition(const Vector3& position);
	void SetRotation(const Vector3& axis, float angle);
	void SetRotation(const Quaternion& rotation);

	/// <summary>
	/// �I�C���[��]
	/// </summary>
	void SetRotation(const Vector3& radian);
	void SetScale(const Vector3& scale);
	void SetColor(const Vector4& color);

	/// <summary>
	/// �G�t�F�N�g��j������B
	/// </summary>
	void Destroy();
	/// <summary>
	/// �\����\��
	/// </summary>
	void SetShown(bool is_shown);

	/// <summary>
	/// �n���h���̃G�t�F�N�g�̃C���X�^���X�����݂��Ă��邩
	/// </summary>
	bool IsExist();

	void ImGuiRender();
private:

	Effekseer::Handle handle;

	Vector3 position;
	Vector3 angle;
	Vector3 scale;
};
