#include "PostEffect.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"
#include "General/MyImGuiHelper.h"

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

void PostEffect::Initialize(
	ID3D11Device* device, 
	uint32_t width, 
	uint32_t height)
{
	this->width = width;
	this->height = height;

	composition_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso");
	composition_shader.CreatePSFromCSO(device, "Shader/PostEffectPS.cso");
	composition_shader.CreateBlendState(device, BSMode::Add);

	composition_rt = std::make_shared<RenderTarget>();
	composition_rt->Create(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �u���[��
	bloom.Initialize(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// �J���[�t�B���^�[
	color_filter.Initialize(device, width, height);

}

static bool is_filter{ true };

void PostEffect::ImGuiRender()
{
	if (ImGui::TreeNode(TO_CHAR(u8"�J���[�t�B���^�[")))
	{
		auto filter{ color_filter.GetFilterParam() };

		ImGui::Checkbox("Filter", &is_filter);

		ImGui::SliderFloat(TO_CHAR(u8"�F��"), &filter.hue_shift, 0.0f, 360.0f);
		ImGui::SliderFloat(TO_CHAR(u8"�ʓx"), &filter.saturation, 0.0f, 2.0f);
		ImGui::SliderFloat(TO_CHAR(u8"���x"), &filter.brightness, 0.0f, 10.0f);
		ImGui::SliderFloat(TO_CHAR(u8"�R���g���X�g"), &filter.contrast, 0.0f, 10.0f);

		color_filter.SetFilterParam(filter);

		ImGui::TreePop();
	}
}

void PostEffect::Apply(
	ID3D11DeviceContext* dc, 
	const Texture& scene_texture)
{


	bloom.Apply(dc, scene_texture);

	Graphics::GetInstance().GetFrameBuffer()->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		composition_rt);

	Graphics::GetInstance().GetTextureRenderer()->BeginDraw(dc, &composition_shader);

	ID3D11ShaderResourceView* srvs[]
	{
		scene_texture.GetView().Get(),
		bloom.GetTexture().GetView().Get()
	};

	Graphics::GetInstance().GetTextureRenderer()->Draw(dc, srvs, _countof(srvs), static_cast<float>(width), static_cast<float>(height));

	if(is_filter) color_filter.Apply(dc, composition_rt->GetTexture());
}

const Texture& PostEffect::GetTexture() const
{
	if (is_filter)
	{
		return color_filter.GetTexture(); 
	}

	return composition_rt->GetTexture();
}
