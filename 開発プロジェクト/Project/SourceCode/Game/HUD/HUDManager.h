#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <d3d11.h>

#include "Graphics/Resource/Texture/Texture.h"

#include "HUDMiniMap.h"
#include "HUDPlayer.h"
#include "HUDDamage.h"
#include "HUDControl.h"
#include "HUDHealth.h"

/* --- 前方宣言 ---- */

class Camera;

/* --- 宣言 ---- */

class HUDManager
{
public:

	void Initialize(ID3D11Device* device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* dc);

	void Reset();

	HUDMiniMap& GetMinimap() { return hud_minimap; }
	HUDPlayer& GetInventory() { return hud_player; }
	HUDDamage& GetDamage() { return hud_damage; }
	HUDControl& GetControl() { return hud_control; }
	HUDHealth& GetHealth() { return hud_health; }

	/// <summary>
	/// HUDの表示設定
	/// </summary>
	void SetShowHUDFlag(bool b) { is_show_hud = b; }

private:

	/// <summary>
	/// （未実装）ターゲットしている敵の目印
	/// </summary>
	void DrawTargetMarker(ID3D11DeviceContext* dc, const Camera* camera, float screen_width, float screen_height);

	bool is_show_hud;

	HUDMiniMap hud_minimap;
	HUDPlayer hud_player;
	HUDDamage hud_damage;
	HUDControl hud_control;
	HUDHealth hud_health;

	Texture target_marker;

	float transparency; // 全体のUIの透明度
	float transparancy_delta;

};
