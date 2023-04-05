#include "DirectionalLight.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- ��` ---- */

DirectionalLight::DirectionalLight(
	const Vector3& light_direction, 
	float luminance, 
	const Vector3& light_color)
{
	light_data = std::make_shared<DeferredLighting::Light>();
	// w���O�ɂ��邱�ƂŃ��C�e�B���O���Ƀ|�C���g���C�g�Ƌ�ʂ�����B
	light_data->light_parameter = Vector4(light_direction, 0.0f);
	light_data->light_color = light_color;
	light_data->luminance = luminance;

	// ������o�^
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
	ImGui::SliderFloat3(TO_CHAR(u8"���̌���"), &light_data->light_parameter.x, -1.0f, 1.0f);
	ImGui::SliderFloat(TO_CHAR(u8"���̋���"), &light_data->luminance, 0.0f, 100.0f);
}

