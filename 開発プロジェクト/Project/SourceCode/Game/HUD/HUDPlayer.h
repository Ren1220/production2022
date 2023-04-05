#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- 宣言 ---- */

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

	static constexpr Vector2 bar_offset  { 10.0f, 10.0f }; // ウィンドウ左上からの距離
	static constexpr Vector2 bar_size{ 600.0f, 30.0f };
	static constexpr float frame_margin_to_bar { 10.0f }; // バー・フレーム間の広さ
	static constexpr float frame_margin{ 10.0f }; // フレーム間の広さ
	static constexpr Vector2 frame_offset{ 10.0f, bar_size.y + frame_margin_to_bar + bar_offset.y }; // ウィンドウ左上からの距離
	static constexpr Vector2 select_frame_size{ 128.0f, 128.0f };
	static constexpr Vector2 frame_size{ 64.0f, 64.0f };


	Texture inventory_frame;
	Texture weapon_icon;
	Texture health_bar;
	Texture health_bar_frame;
};
