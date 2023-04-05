#include "Terrain.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "imgui.h"
#include "General/MyImGuiHelper.h"
#include "../StageData.h"

/* --- 定義 ---- */

Terrain::Terrain()
	: map_width(), map_height()
{
}

void Terrain::Initialize(uint32_t map_width, uint32_t map_height)
{
	this->map_width = map_width;
	this->map_height = map_height;

	// 配列を初期化
	column_row_map = std::vector<std::vector<TerrainTile>>(map_width, std::vector<TerrainTile>(map_height));
}

void Terrain::Import(const char* filepath)
{
	StageData import_data;
	import_data.Import(filepath);

	const auto& header{ import_data.GetHeader() };
	// マップの初期化
	Initialize(header.width, header.height);

	// データを読み取る
	const auto& data{ import_data.GetData() };
	
	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			column_row_map[ci][ri].data = data.data[ci + static_cast<size_t>(ri) * map_width];
		}
	}
}

std::vector<std::pair<uint32_t, uint32_t>> Terrain::FindMapIndices(TileData::State state) const
{
	std::vector<std::pair<uint32_t, uint32_t>> out_indices;
	out_indices.reserve(static_cast<size_t>(map_width * map_height));

	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			if (column_row_map[ci][ri].data.state == state)
			{
				out_indices.emplace_back(ci, ri);
			}
		}
	}

	out_indices.shrink_to_fit();
	return out_indices; 
}

std::vector<std::pair<uint32_t, uint32_t>> Terrain::FindMapIndices(TileData::MeshID state) const
{
	std::vector<std::pair<uint32_t, uint32_t>> out_indices;
	out_indices.reserve(static_cast<size_t>(map_width * map_height));

	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			if (column_row_map[ci][ri].data.mesh == state)
			{
				out_indices.emplace_back(ci, ri);
			}
		}
	}

	out_indices.shrink_to_fit();
	return out_indices;
}

std::vector<std::pair<uint32_t, uint32_t>> Terrain::FindMapIndices(TileData::EntityID state) const
{
	std::vector<std::pair<uint32_t, uint32_t>> out_indices;
	out_indices.reserve(static_cast<size_t>(map_width * map_height));

	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			if (column_row_map[ci][ri].data.entity == state)
			{
				out_indices.emplace_back(ci, ri);
			}
		}
	}

	out_indices.shrink_to_fit();
	return out_indices;
}

std::vector<std::pair<uint32_t, uint32_t>> Terrain::FindMapIndices(TileData::OtherFlag state) const
{
	std::vector<std::pair<uint32_t, uint32_t>> out_indices;
	out_indices.reserve(static_cast<size_t>(map_width * map_height));

	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			if (column_row_map[ci][ri].data.other == state)
			{
				out_indices.emplace_back(ci, ri);
			}
		}
	}

	out_indices.shrink_to_fit();
	return out_indices;
}

std::vector<std::pair<uint32_t, uint32_t>> Terrain::FindMapIndices(TileData::CollideFlag state) const
{
	std::vector<std::pair<uint32_t, uint32_t>> out_indices;
	out_indices.reserve(static_cast<size_t>(map_width * map_height));

	for (uint32_t ri = 0U; ri < map_height; ++ri)
	{
		for (uint32_t ci = 0U; ci < map_width; ++ci)
		{
			if (column_row_map[ci][ri].data.collide == state)
			{
				out_indices.emplace_back(ci, ri);
			}
		}
	}

	out_indices.shrink_to_fit();
	return out_indices;
}
