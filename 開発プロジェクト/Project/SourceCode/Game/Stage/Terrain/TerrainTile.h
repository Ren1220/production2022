#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <array>
#include <memory>

#include "TerrainCell.h"
#include "TileData.h"
#include "TileMaterial.h"

/* --- �錾 ---- */

class TerrainTile
{
public:

	TerrainTile()
		: current_state(), next_state(), data()
	{

	}

	TerrainCell current_state;
	TerrainCell next_state;

	bool IsFloor() const
	{
		if (current_state & (TerrainCell::Alive | TerrainCell::Traces))
		{
			return true;
		}
		else if (data.mesh == TileData::M_Floor)
		{
			return true;
		}

		return false;
	}

	TileData data;
	TileMaterial material;

};
