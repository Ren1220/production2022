#include "SceneTitle.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */


#include "Audio/AudioManager.h"
#include "General/AtaiEngine.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Text/TextRenderer.h"
#include "Input/Input.h"

#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneManager.h"

/* --- ��` ---- */

SceneTitle::SceneTitle()
{
}

void SceneTitle::Initialize()
{
	auto device{ MySystem.GetDevice() };
	const auto [width, height] { MySystem.GetScreenSize<uint32_t>()};

	main_depth_stencil = std::make_shared<DepthStencil>();
	main_render_target = std::make_shared<RenderTarget>();
	main_depth_stencil->Create(device, width, height, DXGI_FORMAT_R32_FLOAT);
	main_render_target->Create(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	sprite_shader.Create(device);
	sprite_shader.CreateBlendState(device, BSMode::Alpha);

	//
	{
		AudioManager::GetInstance().LoadAudioResourcesRecursive(L"Data/Audio");
	}


}

void SceneTitle::EndProcess()
{
}

static int tmp{ 1 };

void SceneTitle::Update(float elapsed_time)
{
	const auto& input{ Input::GetInstance() };
	if (input.GetGamePad().GetButton(GamePad::A, GamePad::DOWN) ||
		input.GetKeyboard().GetKeyState(VK::Z) & Keyboard::Down)
	{
		// �V�����V�[���̐ݒ�
		SceneManager::GetInstance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
		return;
	}

	const auto [widthf, heightf] { MySystem.GetScreenSize<float>() };
	Graphics::GetInstance().GetTextRenderer()->SetText(L"�Q�[���^�C�g��",
		TextRenderer::Format(static_cast<FontFamily>(tmp), 100.0f, DWRITE_TEXT_ALIGNMENT_CENTER), {10.0f, 10.0f}, {widthf - 10.0f, 110.0f}, {1.0f, 1.0f, 1.0f, 1.0f});

	std::wstring text1{ L"�`�{�^�������y�L�[�ŊJ�n" };
	Graphics::GetInstance().GetTextRenderer()->SetText(text1.c_str(), static_cast<FontFamily>(tmp), 50.0f, { widthf - text1.size() * 50.0f + 10.0f , heightf - 60.0f }, { widthf - 10.0f, heightf - 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SceneTitle::Render(ID3D11DeviceContext* dc)
{
	// �����_�[�^�[�Q�b�g�ƃV�F�[�_�[���\�[�X�̓����A�N�Z�X�𖳌�������
	// ���ꂪ�Ȃ��Əo�̓��O�ɃG���[���o��(�t���[���o�b�t�@�g�p��)
	{
		ID3D11RenderTargetView* null_rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
		dc->OMSetRenderTargets(_countof(null_rtvs), null_rtvs, 0);
		ID3D11ShaderResourceView* null_srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
		dc->VSSetShaderResources(0, _countof(null_rtvs), null_srvs);
		dc->PSSetShaderResources(0, _countof(null_rtvs), null_srvs);
		dc->GSSetShaderResources(0, _countof(null_rtvs), null_srvs);
	}

	auto& graphics{ Graphics::GetInstance() };
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer() };

	const auto [widthf, heightf] { MySystem.GetScreenSize<float>() };
	frame_buffer->Set(dc, widthf, heightf, main_depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, main_render_target);


	MySystem.ActivateSwapChainBuffer();
	// �Q�[�����
	graphics.GetTextureRenderer()->BeginDraw(dc);
	graphics.GetTextureRenderer()->Draw(dc, main_render_target->GetTexture());

	graphics.GetTextRenderer()->Render(MySystem.GetDeviceContext2D());
}

void SceneTitle::ImGuiRender()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("TextFormat"))
	{
		ImGui::SliderInt("FontFamily", &tmp, 0, static_cast<int>(FontFamily::ElementNum) - 1);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Audio"))
	{
		AudioManager::GetInstance().ImGuiRender();

		ImGui::EndMenu();
	}


	ImGui::EndMainMenuBar();
}

void SceneTitle::ResetScreen()
{
}
