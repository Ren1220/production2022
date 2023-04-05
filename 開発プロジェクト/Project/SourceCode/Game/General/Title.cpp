#include "Title.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/General/GameSystem.h"
#include "Game/General/GameText.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- 定義 ---- */

void TitleSystem::Initialize(ID3D11Device* device)
{
	is_title = true;
	text_frame.Create(device, L"Data/Texture/HUD/Text/text_frame.png");
	title.Create(device, L"Data/Texture/Title/title.png");
}

void TitleSystem::Render(ID3D11DeviceContext* dc)
{
	if (!is_title) return;

	using namespace std::string_view_literals;

	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	auto& game_text{ GameSys.GetGameText() };

	constexpr auto wsv{ L"Aを押してゲームを開始する"sv };
	constexpr float font_size{ 20.0f };
	const float frame_width{ wsv.length() * 40.0f + 60.0f };

	// タイトルロゴ
	texture_renderer->BeginDraw(dc, BSMode::Alpha);
	texture_renderer->Draw(dc, title, { 800.0f, 300.0f }, { static_cast<float>(title.GetDesc().Width), static_cast<float>(title.GetDesc().Height) }, { 1.0f, 1.0f, 1.0f, 1.0f }, TexturePivot::Center);
	texture_renderer->EndDraw(dc);

	// テキストフレームの描画
	texture_renderer->BeginDraw(dc);
	texture_renderer->Draw(dc, text_frame, { 800.0f - (frame_width * 0.5f), 700.0f }, { frame_width, 200.0f }, { 0.0f, 0.0f }, { 256.0f, 256.0f }, { 0.5f, 0.5f, 0.5f, 0.5f }, TexturePivot::LeftTop);
	texture_renderer->EndDraw(dc);

	// テキストの描画
	game_text->SetText(GameText::Layer::HUD, wsv.data(), FontFamily::Kokumincho, 40, { 800.0f - (frame_width * 0.5f), 700.0f }, { frame_width, 200.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}
