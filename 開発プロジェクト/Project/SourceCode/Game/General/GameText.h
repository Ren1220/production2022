#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <array>
#include <vector>

#include "Graphics/Text/TextRenderer.h"

/* --- 宣言 ---- */

/// <summary>
/// 主に画面フェードの影響を受けるテキストと受けないテキストを分けるために作られたクラス。
/// このクラスを介して、TextRendererにテキストを設定して描画する。
/// </summary>
class GameText
{
public:

	enum class Layer
	{
		GS,  // ゲームシーン。フェードの影響を受ける
		HUD, // フェードの影響を受けない
		Opt, // オプション画面。
		ElementCount,
	};
	
	void Initialize();

	/// <summary>
	/// テキストを送信する、各レイヤーの描画処理は適切なタイミングでまとめて行う。
	/// </summary>
	void SetText(
		Layer layer,
		const wchar_t* text,
		FontFamily family,
		FLOAT font_size,
		const D2D1_POINT_2F& point,
		const D2D1_SIZE_F& size,
		const D2D1_COLOR_F& color,
		DWRITE_TEXT_ALIGNMENT text_align = DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT para_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	/// <summary>
	/// テキストを送信する、各レイヤーの描画処理は適切なタイミングでまとめて行う。
	/// </summary>
	void SetText(
		Layer layer,
		const wchar_t* text,
		TextRenderer::Format format,
		const D2D1_POINT_2F& point,
		const D2D1_SIZE_F& size,
		const D2D1_COLOR_F& color);

	/// <summary>
	/// レイヤーのインデックス内のテキストを描画する
	/// </summary>
	void Render(ID2D1DeviceContext* dc, Layer layer);

private:

	std::array<std::vector<TextRenderer::Text>, static_cast<size_t>(Layer::ElementCount)> texts;
};
