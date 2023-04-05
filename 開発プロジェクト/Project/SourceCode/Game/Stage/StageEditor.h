#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Graphics/PostProcess/DeferredLighting.h"

#include "Stage.h"

/* --- �O���錾 ---- */

class OrthographicCamera;
class Camera;

/* --- �錾 ---- */

class StageEditor
{
public:

	StageEditor();
	~StageEditor();

	/// <summary>
	/// �G�f�B�^�̊J�n����
	/// </summary>
	void Start();
	
	/// <summary>
	/// �G�f�B�^�̏I������
	/// </summary>
	void End();

	void Update(float elapsed_time, Stage& out_stage);

	void ImGuiRender(Stage& out_stage);

private:

	void ContorlCamera(float elapsed_time);

	void Create(uint32_t width, uint32_t height, Stage& out_stage);
	void Export(const Stage& stage);
	void Import(Stage& out_stage);


	void MarkEntitySpawnPoint(const Stage& stage);
	
	void SelectCell(const Stage& stage);
	void CheckCell(const Stage& out_stage);
	void ChangeCell(Stage& out_stage);

	std::pair<int64_t, int64_t> selecting_cell;

	std::shared_ptr<OrthographicCamera> camera;
	std::shared_ptr<Camera> tmp_camera; // �Q�[�����Ŏg�p����J����
	std::shared_ptr<DeferredLighting::Light> light;
};
