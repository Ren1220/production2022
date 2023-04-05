#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "scene.h"
#include "Graphics/Resource/Texture/TextureAnimation.h"

/* --- �錾 ---- */

class SceneLoading : public Scene
{
public:

	SceneLoading(std::unique_ptr<Scene>&& next_scene);
	~SceneLoading() = default;

	
public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;

private:

	/// <summary>
	/// �ʃX���b�h�ōs�������A
	/// Com�I�u�W�F�N�g�A���̃V�[���̏��������s���B
	/// ��������ƁAscene��is_ready��ture�ɐݒ肷��B
	/// </summary>
	/// <param name="scene"></param>
	static void LoadingThread(SceneLoading* scene);


private:

	std::unique_ptr<TextureAnimation> loading;

	std::unique_ptr<Scene> next_scene{ nullptr };

};