#include "DirectionalLight.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- 定義 ---- */

DirectionalLight::DirectionalLight(
	const Vector3& light_direction, 
	float luminance, 
	const Vector3& light_color)
{
	light_data = std::make_shared<DeferredLighting::Light>();
	// wを０にすることでライティング時にポイントライトと区別をする。
	light_data->light_parameter = Vector4(light_direction, 0.0f);
	light_data->light_color = light_color;
	light_data->luminance = luminance;

	// 光源を登録
	Graphics::GetInstance().GetDeferredLighting()->RegisterLight(light_data);
}

void DirectionalLight::Start()
{
}

void DirectionalLight::Update(float elapsed_time)
{
}

void DirectionalLight::ImGuiRender()
{
	ImGui::SliderFloat3(TO_CHAR(u8"光の向き"), &light_data->light_parameter.x, -1.0f, 1.0f);
	ImGui::SliderFloat(TO_CHAR(u8"光の強さ"), &light_data->luminance, 0.0f, 100.0f);
}

