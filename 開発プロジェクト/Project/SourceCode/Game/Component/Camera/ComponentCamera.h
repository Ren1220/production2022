#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Component.h"
#include "Graphics/Resource/Camera/Camera.h"

/* --- 宣言 ---- */

class ComponentCamera : public Component
{
public:

	ComponentCamera(const std::shared_ptr<Camera>& camera, bool is_main_camera);
	~ComponentCamera() override = default;

	/// <summary>
	/// 開始処理
	/// </summary>
	void Start() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;
	
	/// <summary>
	/// 定数バッファで参照するカメラ情報としてこのカメラを使用する
	/// </summary>
	void SetToFrameCamera();

	bool IsMainCamera() const { return is_main_camera; }

private:

	std::shared_ptr<Camera> camera;

	bool is_main_camera;

};
