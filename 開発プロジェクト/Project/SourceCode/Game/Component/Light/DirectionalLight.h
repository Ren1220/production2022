#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Component.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- 宣言 ---- */

class DirectionalLight : public Component
{
public:

	DirectionalLight(const Vector3& light_direction, float luminance, const Vector3& light_color = { 1, 1, 1 });
	virtual ~DirectionalLight() override = default;

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

private:

	std::shared_ptr<DeferredLighting::Light> light_data;
};
