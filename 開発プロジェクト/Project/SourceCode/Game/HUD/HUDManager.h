#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <d3d11.h>

#include "Graphics/Resource/Texture/Texture.h"

#include "HUDMiniMap.h"
#include "HUDPlayer.h"
#include "HUDDamage.h"
#include "HUDControl.h"
#include "HUDHealth.h"

/* --- �O���錾 ---- */

class Camera;

/* --- �錾 ---- */

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
	/// HUD�̕\���ݒ�
	/// </summary>
	void SetShowHUDFlag(bool b) { is_show_hud = b; }

private:

	/// <summary>
	/// �i�������j�^�[�Q�b�g���Ă���G�̖ڈ�
	/// </summary>
	void DrawTargetMarker(ID3D11DeviceContext* dc, const Camera* camera, float screen_width, float screen_height);

	bool is_show_hud;

	HUDMiniMap hud_minimap;
	HUDPlayer hud_player;
	HUDDamage hud_damage;
	HUDControl hud_control;
	HUDHealth hud_health;

	Texture target_marker;

	float transparency; // �S�̂�UI�̓����x
	float transparancy_delta;

};
