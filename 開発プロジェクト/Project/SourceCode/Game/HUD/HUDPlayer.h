#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- �錾 ---- */

class HUDPlayer
{
public:

	HUDPlayer();
	~HUDPlayer();

	void Initialize(ID3D11Device* device);

	void Render(ID3D11DeviceContext* dc, float transparency);

private:

	void RenderHealth(ID3D11DeviceContext* dc, float screen_width, float screen_height, float transparency);
	void RenderInventory(ID3D11DeviceContext* dc, float screen_width, float screen_height, float transparency);

	static constexpr Vector2 bar_offset  { 10.0f, 10.0f }; // �E�B���h�E���ォ��̋���
	static constexpr Vector2 bar_size{ 600.0f, 30.0f };
	static constexpr float frame_margin_to_bar { 10.0f }; // �o�[�E�t���[���Ԃ̍L��
	static constexpr float frame_margin{ 10.0f }; // �t���[���Ԃ̍L��
	static constexpr Vector2 frame_offset{ 10.0f, bar_size.y + frame_margin_to_bar + bar_offset.y }; // �E�B���h�E���ォ��̋���
	static constexpr Vector2 select_frame_size{ 128.0f, 128.0f };
	static constexpr Vector2 frame_size{ 64.0f, 64.0f };


	Texture inventory_frame;
	Texture weapon_icon;
	Texture health_bar;
	Texture health_bar_frame;
};
