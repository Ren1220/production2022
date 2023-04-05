#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>

#include "Graphics/Resource/Texture/Texture.h"

/* --- 前方宣言 ---- */

class Camera;

/* --- 宣言 ---- */

class HUDHealth
{
public:

	void Initialize(ID3D11Device* device);

	void Reset();

	void Update(float elapsed_time);

	void Render(
		ID3D11DeviceContext* dc, 
		const Camera* camera, 
		float screen_width, 
		float screen_height);

	void SetShowBossHealthFlag(bool b) { show_boss_health = b; }

private:

	Texture health_bar;
	Texture boss_health_bar;
	Texture boss_health_bar_frame;
	Texture low_health_direction;
	bool show_boss_health;

	float timer;
};
