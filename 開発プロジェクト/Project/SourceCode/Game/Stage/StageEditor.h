#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Graphics/PostProcess/DeferredLighting.h"

#include "Stage.h"

/* --- 前方宣言 ---- */

class OrthographicCamera;
class Camera;

/* --- 宣言 ---- */

class StageEditor
{
public:

	StageEditor();
	~StageEditor();

	/// <summary>
	/// エディタの開始処理
	/// </summary>
	void Start();
	
	/// <summary>
	/// エディタの終了処理
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
	std::shared_ptr<Camera> tmp_camera; // ゲーム内で使用するカメラ
	std::shared_ptr<DeferredLighting::Light> light;
};
