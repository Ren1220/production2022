#include "TextRenderer.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Include/misc.h"
#include "Debug/DebugGUI.h"

/* --- 定義 ---- */

void TextRenderer::Initialize()
{
	HRESULT hr{ S_OK };

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), reinterpret_cast<IUnknown**>(dwrite_factory.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// フォントファイルからフォントを読み込む

	// https://deep-verdure.hatenablog.com/entry/2022/07/23/190449
	Microsoft::WRL::ComPtr<IDWriteFontSetBuilder1> font_set_builder;
	hr = dwrite_factory->CreateFontSetBuilder(font_set_builder.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	CreateTextFormat(font_set_builder.Get(), FontFamily::Mariyo, L"Data/Font/MeiryoUI/meiryo.ttc", L"Meiryo UI");
	CreateTextFormat(font_set_builder.Get(), FontFamily::SoukouMincho, L"Data/Font/SoukouMincho-Font/SoukouMincho.ttf", L"SoukouMincho");
	CreateTextFormat(font_set_builder.Get(), FontFamily::Mutsuki, L"Data/Font/Mutsuki/Mutsuki.ttf", L"Mutsuki");
	CreateTextFormat(font_set_builder.Get(), FontFamily::MaruMonica, L"Data/Font/x12y16pxMaruMonica_2021-06-17/x12y16pxMaruMonica.ttf", L"x12y16pxMaruMonica");
	CreateTextFormat(font_set_builder.Get(), FontFamily::Makinas4, L"Data/Font/makinas4/Makinas-4-Square.otf", L"Makinas 4 Square");
	CreateTextFormat(font_set_builder.Get(), FontFamily::Kokumincho, L"Data/Font/ki_kokumin/font_1_kokumr_1.00_rls.ttf", L"刻明朝 Regular");

	texts.clear();
}

void TextRenderer::SetText(
	const wchar_t* text, 
	FontFamily family, 
	FLOAT font_size, 
	const D2D1_POINT_2F& point, 
	const D2D1_SIZE_F& size, 
	const D2D1_COLOR_F& color, 
	DWRITE_TEXT_ALIGNMENT text_align,
	DWRITE_PARAGRAPH_ALIGNMENT para_align)
{
	SetText(text, Format(family, font_size, text_align, para_align), point, size, color);
}

void TextRenderer::SetText(
	const wchar_t* text, 
	Format format, 
	const D2D1_POINT_2F& point,
	const D2D1_SIZE_F& size,
	const D2D1_COLOR_F& color)
{
	texts.emplace_back(text, format, point, size, color);
}

void TextRenderer::Render(ID2D1DeviceContext* dc)
{
	dc->BeginDraw();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;		// 描画色

	for (const auto& text : texts)
	{
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout;
		CreateTextLayout(text, layout.ReleaseAndGetAddressOf());

		// 処理負荷はほとんど無いらしい
		HRESULT hr = dc->CreateSolidColorBrush(text.color, solid_color_brush.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		dc->DrawTextLayout(text.point, layout.Get(), solid_color_brush.Get());
	}

	dc->EndDraw();

	texts.clear();
}

void TextRenderer::Render(
	ID2D1DeviceContext* dc,
	const Text* data, 
	size_t sz)
{
	dc->BeginDraw();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;		// 描画色

	for(size_t ti = 0ULL; ti < sz; ++ti)
	{
		const auto& text{ data[ti] };

		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout;
		CreateTextLayout(text, layout.ReleaseAndGetAddressOf());

		// 処理負荷はほとんど無いらしい
		HRESULT hr = dc->CreateSolidColorBrush(text.color, solid_color_brush.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		dc->DrawTextLayout(text.point, layout.Get(), solid_color_brush.Get());
	}

	dc->EndDraw();

	texts.clear();
}

void TextRenderer::CreateTextLayout(const Text& text, IDWriteTextLayout** out_text_layout)
{
	HRESULT hr = dwrite_factory->CreateTextLayout(
		text.wstr.data(), static_cast<UINT32>(text.wstr.size()), 
		text_formats[static_cast<size_t>(text.format.family)].Get(),
		text.size.width,
		text.size.height,
		out_text_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//hr = text_formats[static_cast<size_t>(text.format.family)]->SetTextAlignment(text.format.alignment);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// DWRITE_TEXT_RANGEで個々の値で何列目のフォントからのみの変更等ができるがそこまで細かく編集はしない
	const DWRITE_TEXT_RANGE range{ 0U, static_cast<UINT32>(text.wstr.size()) };
	
	hr = (*out_text_layout)->SetFontSize(text.format.size, range);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = (*out_text_layout)->SetFontWeight(text.format.weight, range);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = (*out_text_layout)->SetFontStyle(text.format.style, range);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = (*out_text_layout)->SetFontStretch(text.format.stretch, range);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = (*out_text_layout)->SetTextAlignment(text.format.text_align);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = (*out_text_layout)->SetParagraphAlignment(text.format.para_align);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void TextRenderer::CreateTextFormat(
	IDWriteFontSetBuilder1* font_set_builder, 
	FontFamily font_family,
	const wchar_t* filepath, 
	const wchar_t* font_family_name)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDWriteFontFile> font_file;

	hr = dwrite_factory->CreateFontFileReference(
		filepath,
		nullptr,
		font_file.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = font_set_builder->AddFontFile(font_file.Get());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<IDWriteFontSet> font_set;
	hr = font_set_builder->CreateFontSet(font_set.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<IDWriteFontCollection1> font_collection;
	hr = dwrite_factory->CreateFontCollectionFromFontSet(
		font_set.Get(),
		font_collection.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = dwrite_factory->CreateTextFormat(
		font_family_name,
		font_collection.Get(),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		1.0f,
		L"",
		text_formats[static_cast<size_t>(font_family)].ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
