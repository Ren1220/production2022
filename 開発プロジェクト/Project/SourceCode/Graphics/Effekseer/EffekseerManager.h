#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <unordered_map>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "Arithmetic/Arithmetic.h"

#include "EffekseerEffect.h"

/* --- ��` ---- */

class EffekseerManager
{
public:


	EffekseerManager() {}	
	~EffekseerManager() {}

	/// <summary>
	/// ����������
	/// �����Ńf�o�C�X�R���e�L�X�g���g�p���Ă��邽�߁A
	/// �~���[�e�b�N�X�ɂ���ă}���`�X���b�h�̓����A�N�Z�X��h���ł���B
	/// </summary>
	void Initialize(ID3D11Device* device);
	/// <summary>
	/// �I������
	/// </summary>
	void EndProcess();

	void Update(float elapsed_time);
	void Render(const Matrix& view, const Matrix& projection);
	void ImGuiRender();

	/// <summary>
	/// �S�ẴG�t�F�N�g���~������
	/// </summary>
	void StopAllEffects();
	/// <summary>
	/// �n���h���̃G�t�F�N�g���~����
	/// </summary>
	void StopEffect(Effekseer::Handle handle);
	/// <summary>
	/// �n���h���̃G�t�F�N�g��`�悷�邩
	/// </summary>
	void SetShownEffect(Effekseer::Handle handle, bool is_shown);

	Effekseer::Manager* GetEffekseerManager() { return effekseer_manager; }

	/// <summary>
	/// �����̃t�@�C���p�X�̃G�t�F�N�g�𔭐�������
	/// </summary>
	/// <param name="filepath">"Data/Effect/"�ɑ����t�@�C���p�X</param>
	/// <returns>�����������G�t�F�N�g�𑀍삷�邽�߂̃n���h��ID</returns>
	Effekseer::Handle PlayEffect(const char* filepath, const Vector3& position);

	/// <summary>
	/// �G�t�F�N�g�𐶐�����A�\�ߐ������邱�ƂŃQ�[�����̓ǂݍ��݂̔����������
	/// </summary>
	/// <param name="filepath">"Data/Effect/"�ɑ����t�@�C���p�X</param>
	/// <returns>�����������G�t�F�N�g�𑀍삷�邽�߂̃n���h��ID</returns>
	void LoadEffect(const char* filepath);

	/// <summary>
	/// �����̃f�B���N�g���ȉ���efk�t�@�C�������ׂēǂݍ���
	/// </summary>
	/// <param name="directory"></param>
	/// <returns></returns>
	void LoadEffectRecursive(const wchar_t* directory_path);

private:



	Effekseer::Manager* effekseer_manager{ nullptr };
	EffekseerRenderer::Renderer* effekseer_renderer{ nullptr };

	std::unordered_map<std::string, EffekseerEffect> effects;

};