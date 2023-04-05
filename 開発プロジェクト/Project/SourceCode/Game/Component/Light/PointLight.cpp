#include "PointLight.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/General/Title.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- 定義 ---- */

PointLight::PointLight(const Vector3& light_position, float attenuation, const Vector3& light_color, float luminance, bool is_shadow_cast)
	: attenuation_delta(attenuation), luminance_delta(luminance), is_shadow_cast(is_shadow_cast)
{
	light_data = std::make_shared<DeferredLighting::Light>();
	light_data->light_parameter = Vector4(light_position, attenuation / 3);
	light_data->light_color = light_color;
	light_data->luminance = luminance;
}

void PointLight::Start()
{
	Graphics::GetInstance().GetDeferredLighting()->RegisterLight(light_data);
}

void PointLight::Update(float elapsed_time)
{
	if (!TitleSystem::GetInstance().IsTitle())
	{
		light_data->luminance = std::lerp(light_data->luminance, luminance_delta, elapsed_time);
		light_data->light_parameter.w = std::lerp(light_data->light_parameter.w, attenuation_delta, elapsed_time);
	}


	auto actor{ GetActor() };
	light_data->light_parameter.x = actor->GetWorldPosition().x;
	light_data->light_parameter.y = actor->GetWorldPosition().y;
	light_data->light_parameter.z = actor->GetWorldPosition().z;
}

void PointLight::ImGuiRender()
{
	ImGui::SliderFloat("Luminance", &light_data->luminance, 0.0f, 1000.0f, "%.1f");
	ImGui::SliderFloat("Attenuation", &light_data->light_parameter.w, 0.01f, 100.0f);

	ImGui::ColorEdit3("Color", &light_data->light_color.x);
}
