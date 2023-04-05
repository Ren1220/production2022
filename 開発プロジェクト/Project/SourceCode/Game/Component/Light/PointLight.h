#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Component.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- 宣言 ---- */

class PointLight : public Component
{
public:

	PointLight(const Vector3& light_position, float attenuation, const Vector3& light_color, float luminance, bool is_shadow_cast);
	~PointLight() override = default;

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

	// タイトルでは真っ暗にするため、一次的に初期値を個々に確保する
	float attenuation_delta;
	float luminance_delta;

	bool is_shadow_cast; // この光源でシャドウを行うか

};
