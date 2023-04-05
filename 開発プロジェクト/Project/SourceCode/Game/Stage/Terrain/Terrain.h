#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <vector>
#include <string>

#include "Arithmetic/Arithmetic.h"

#include "TerrainTile.h"
#include "TerrainData.h"

/* --- �錾 ---- */

class Terrain
{
public:


	Terrain();

	/// <summary>
	/// �������A�}�b�v�T�C�Y��ݒ肷��B
	/// </summary>
	void Initialize(uint32_t map_width, uint32_t map_height);

	/// <summary>
	/// �t�@�C������n�`�f�[�^��ǂݎ��B
	/// �ǂݎ�����f�[�^��Map��current_state�ɏ������ށB
	/// </summary>
	/// <param name="filepath"></param>
	void Import(const char* filepath);

	const std::vector<std::vector<TerrainTile>>& GetMap() const { return column_row_map; }
	std::vector<std::vector<TerrainTile>>& GetMap() { return column_row_map; }
	
	uint32_t GetMapWidth() const { return map_width; }
	uint32_t GetMapHeight() const { return map_height; }

	Matrix CalcWorldTransform(uint32_t column, uint32_t row) const
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			static_cast<float>(column), 0.0f, static_cast<float>(row), 1.0f
		};
	}

	std::vector<std::pair<uint32_t, uint32_t>> FindMapIndices(TileData::State state) const;
	std::vector<std::pair<uint32_t, uint32_t>> FindMapIndices(TileData::MeshID state) const;
	std::vector<std::pair<uint32_t, uint32_t>> FindMapIndices(TileData::EntityID state) const;
	std::vector<std::pair<uint32_t, uint32_t>> FindMapIndices(TileData::OtherFlag state) const;
	std::vector<std::pair<uint32_t, uint32_t>> FindMapIndices(TileData::CollideFlag state) const;

	/// <summary>
	/// �n�`�������̏���ݒ肷��
	/// </summary>
	void SetTerrainData(const TerrainData& d) { terrain_data = d; }

	/// <summary>
	/// �n�`�������̏����擾����
	/// </summary>
	const TerrainData& GetTerrainData() const { return terrain_data; }

private:

	uint32_t map_width;
	uint32_t map_height;

	std::vector<std::vector<TerrainTile>> column_row_map; // ��*�c�̒n�`���

	TerrainData terrain_data;

#ifdef _DEBUG
	std::vector<std::u8string> _stage;
#endif // _DEBUG


};