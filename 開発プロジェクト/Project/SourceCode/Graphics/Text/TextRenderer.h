#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <unordered_map>
#include <array>
#include <string>

#include <wrl.h>
#include <d2d1_1.h>
#include <dwrite_3.h>

/* --- 宣言 ---- */


enum class FontFamily
{
	Mariyo,
	SoukouMincho,
	Mutsuki,
	MaruMonica,
	Makinas4,
	Kokumincho,
	ElementNum,
};

/// <summary>
/// DirectX2Dによるテキスト描画クラス
/// </summary>
class TextRenderer
{
public:

	TextRenderer() = default;
	~TextRenderer() = default;

	// テキストフォーマット
	struct Format
	{
		Format() = default;

		Format(
			FontFamily family,
			FLOAT size,
			DWRITE_TEXT_ALIGNMENT text_align = DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT para_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
			DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL)
			: family(family), text_align(text_align), para_align(para_align), 
			  weight(weight), style(style), stretch(stretch), size(size) {}

		FontFamily family;
		DWRITE_TEXT_ALIGNMENT text_align;
		DWRITE_PARAGRAPH_ALIGNMENT para_align; 
		DWRITE_FONT_WEIGHT weight;
		DWRITE_FONT_STYLE style;
		DWRITE_FONT_STRETCH stretch;
		FLOAT size;
	};

	struct Text
	{
		std::wstring wstr;
		Format format;
		D2D1_POINT_2F point;
		D2D1_SIZE_F size;
		D2D1_COLOR_F color;
	};

	void Initialize();

	void SetText(
		const wchar_t* text,
		FontFamily family,
		FLOAT font_size,
		const D2D1_POINT_2F& point,
		const D2D1_SIZE_F& size,
		const D2D1_COLOR_F& color,
		DWRITE_TEXT_ALIGNMENT text_align = DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT para_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	void SetText(
		const wchar_t* text,
		Format format,
		const D2D1_POINT_2F& point,
		const D2D1_SIZE_F& size,
		const D2D1_COLOR_F& color);

	void Render(ID2D1DeviceContext* dc);

	/// <summary>
	/// 外部のテキストデータの描画
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="data"></param>
	/// <param name="size"></param>
	void Render(ID2D1DeviceContext* dc, const Text* data, size_t sz);

	void CreateTextLayout(const Text& text, IDWriteTextLayout** out_text_layout);

private:

	/// <summary>
	/// ファイルからフォントを生成する
	/// </summary>
	/// <param name="font_set_builder"></param>
	/// <param name="font_family"></param>
	/// <param name="filepath"></param>
	/// <param name="font_family_name"></param>
	void CreateTextFormat(
		IDWriteFontSetBuilder1* font_set_builder,
		FontFamily font_family,
		const wchar_t* filepath,
		const wchar_t* font_family_name);

	std::array<Microsoft::WRL::ComPtr<IDWriteTextFormat>, static_cast<size_t>(FontFamily::ElementNum)> text_formats;		
	std::vector<Text> texts;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout;
	Microsoft::WRL::ComPtr<IDWriteFactory5> dwrite_factory;				// テキストフォーマット作成オブジェクト


};
