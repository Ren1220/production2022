#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <cstdint>

#include "Graphics/Resource/Texture/Texture.h"

/* --- 宣言 ---- */

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
		Passed	= 1 << 1, // 一度通った
		Showing = 1 << 2, // 視界範囲
	};

	std::vector<std::vector<CellState>> minimap_cell_state;
	
	uint32_t map_size;
	float frame_size;

	Texture minimap_frame;
	Texture terrain_tile;
	Texture entity_tile;

};
