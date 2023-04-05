#include "GameText.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"

/* --- ��` ---- */

void GameText::Initialize()
{
	for (auto& t : texts) { t.clear(); }
}

void GameText::SetText(
	Layer layer, 
	const wchar_t* text,
	FontFamily family, 
	FLOAT font_size, 
	const D2D1_POINT_2F& point, 
	const D2D1_SIZE_F& size,
	const D2D1_COLOR_F& color, 
	DWRITE_TEXT_ALIGNMENT text_align, 
	DWRITE_PARAGRAPH_ALIGNMENT para_align)
{
	SetText(layer, text, TextRenderer::Format(family, font_size, text_align, para_align), point, size, color);
}

void GameText::SetText(Layer layer, const wchar_t* text, TextRenderer::Format format, const D2D1_POINT_2F& point, const D2D1_SIZE_F& size, const D2D1_COLOR_F& color)
{
	texts[static_cast<size_t>(layer)].emplace_back(text, format, point, size, color);
}

void GameText::Render(ID2D1DeviceContext* dc, Layer layer)
{
	auto& t{ texts[static_cast<size_t>(layer)] };
	Graphics::GetInstance().GetTextRenderer()->Render(dc, t.data(), t.size());

	// �`�悵����z��̏�����
	t.clear();
}
