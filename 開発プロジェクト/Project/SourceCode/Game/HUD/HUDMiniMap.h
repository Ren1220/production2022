#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <cstdint>

#include "Graphics/Resource/Texture/Texture.h"

/* --- �錾 ---- */

class HUDMiniMap
{
public:

	HUDMiniMap()
		: map_size(20U), frame_size(300.0f) {};
	virtual ~HUDMiniMap() = default;

	void Initialize(ID3D11Device* device);

	void Reset();

	void UpdateMimimap();

	void Render(ID3D11DeviceContext* dc, float transparency);

	void ImGuiRender();

private:

	enum CellState : uint8_t
	{
		None	= 0,	  // 
		Passed	= 1 << 1, // ��x�ʂ���
		Showing = 1 << 2, // ���E�͈�
	};

	std::vector<std::vector<CellState>> minimap_cell_state;
	
	uint32_t map_size;
	float frame_size;

	Texture minimap_frame;
	Texture terrain_tile;
	Texture entity_tile;

};
