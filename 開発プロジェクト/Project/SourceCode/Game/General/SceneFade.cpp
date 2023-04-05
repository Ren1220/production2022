#include "SceneFade.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

void AreaChange::Initialize(ID3D11Device* device, float initial_fade)
{
	fade_texture.Create(device);

	fade = fade_prime = initial_fade;
}

void AreaChange::Update(float elapsed_time)
{
	// �ǂݍ��݌��elapsed���傫���Ȃ�̂ł��̑΍�
	elapsed_time = (std::min)(elapsed_time, 0.01666f);

	fade = std::lerp(fade, fade_prime, fade_delta * elapsed_time);
	fade = std::clamp(fade, 0.0f, 1.0f);
}


void AreaChange::Render(ID3D11DeviceContext* dc, float width, float height)
{
	auto& renderer{ Graphics::GetInstance().GetTextureRenderer() };
	renderer->BeginDraw(dc);
	renderer->Draw(dc, fade_texture, { 0.0f, 0.0f }, { width, height }, { 0, 0 }, { 1, 1 }, { 0.0f, 0.0f, 0.0f, fade }, TexturePivot::LeftTop);
	renderer->EndDraw(dc);
}

bool AreaChange::IsFaded(float tol)
{
	return fabsf(fade_prime - fade) < tol;
}
