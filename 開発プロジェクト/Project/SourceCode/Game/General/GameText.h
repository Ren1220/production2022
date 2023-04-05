#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <array>
#include <vector>

#include "Graphics/Text/TextRenderer.h"

/* --- �錾 ---- */

/// <summary>
/// ��ɉ�ʃt�F�[�h�̉e�����󂯂�e�L�X�g�Ǝ󂯂Ȃ��e�L�X�g�𕪂��邽�߂ɍ��ꂽ�N���X�B
/// ���̃N���X����āATextRenderer�Ƀe�L�X�g��ݒ肵�ĕ`�悷��B
/// </summary>
class GameText
{
public:

	enum class Layer
	{
		GS,  // �Q�[���V�[���B�t�F�[�h�̉e�����󂯂�
		HUD, // �t�F�[�h�̉e�����󂯂Ȃ�
		Opt, // �I�v�V������ʁB
		ElementCount,
	};
	
	void Initialize();

	/// <summary>
	/// �e�L�X�g�𑗐M����A�e���C���[�̕`�揈���͓K�؂ȃ^�C�~���O�ł܂Ƃ߂čs���B
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
	/// �e�L�X�g�𑗐M����A�e���C���[�̕`�揈���͓K�؂ȃ^�C�~���O�ł܂Ƃ߂čs���B
	/// </summary>
	void SetText(
		Layer layer,
		const wchar_t* text,
		TextRenderer::Format format,
		const D2D1_POINT_2F& point,
		const D2D1_SIZE_F& size,
		const D2D1_COLOR_F& color);

	/// <summary>
	/// ���C���[�̃C���f�b�N�X���̃e�L�X�g��`�悷��
	/// </summary>
	void Render(ID2D1DeviceContext* dc, Layer layer);

private:

	std::array<std::vector<TextRenderer::Text>, static_cast<size_t>(Layer::ElementCount)> texts;
};
