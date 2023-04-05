#pragma once

/* --- �T�v ---- */

// �n�`�f�[�^����擾�����������ƂɁA�n�`�̃C���X�^���X�`����s���B
// �}�e���A���̓x�[�X�ƂȂ郂�f������쐬�������̂��璊�o�������𗘗p����B
// ���������Ƀe�N�X�`���z����쐬���邱�ƂŁA�`�悷��}�e���A����؂�ւ���B

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>
#include <filesystem>

#include "Graphics/Shader/Shader.h"

#include "Terrain.h"
#include "TerrainMesh.h"

/* --- �錾 ---- */

class TerrainRenderer
{
public:

	TerrainRenderer() = default;
	~TerrainRenderer() = default;

	void Initialize(
		ID3D11Device* device, 
		uint32_t max_instance_count = 10000U);

	/// <summary>
	/// �e���b�V���̃C���X�^���X�`����s���A
	/// �}�e���A���͑Ή�����}�b�v�̃^�C������ݒ肳���B
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="shader"></param>
	/// <param name="terrain"></param>
	void Render(ID3D11DeviceContext* dc, Shader* shader, const Terrain& terrain);

	const TerrainMesh& GetFloorMesh() const { return floor_mesh; }
	const TerrainMesh& GetWallMesh() const { return wall_mesh; }

private:

	void InitializeMaterials(ID3D11Device* device);

	// �C���X�^���X�`����s�����b�V�����f���̃t�@�C���p�X
	static constexpr const char* tile_floor_mesh_filepath       { "Data/Model/GLTF/Tiles/_Base/Floor/Default/floor.gltf" };
	static constexpr const char* tile_wall_mesh_filepath        { "Data/Model/GLTF/Tiles/_Base/Wall/Default/wall.gltf" };
	static constexpr const char* tile_wall_ceiling_mesh_filepath{ "Data/Model/GLTF/Tiles/_Base/WallCeiling/Default/ceiling.gltf" };

	TerrainMesh floor_mesh;
	TerrainMesh wall_mesh;
	TerrainMesh wall_ceiling_mesh;

};
