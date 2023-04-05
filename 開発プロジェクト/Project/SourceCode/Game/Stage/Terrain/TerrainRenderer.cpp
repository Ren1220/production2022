#include "TerrainRenderer.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */


#include "General/AtaiEngine.h"

/* --- ��` ---- */

void TerrainRenderer::Initialize(ID3D11Device* device, uint32_t max_instance_count)
{
	// ��
	floor_mesh.Initialize(device, TileData::M_Floor, tile_floor_mesh_filepath, max_instance_count);
	floor_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Floor::Default), "Data/Model/GLTF/Tiles/1/Floor/Default/floor.gltf");
	floor_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Floor::Warp), "Data/Model/GLTF/Tiles/1/Floor/Warp/floor.gltf");

	// ��
	wall_mesh.Initialize(device, TileData::M_Wall, tile_wall_mesh_filepath, max_instance_count);
	wall_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Wall::Default), "Data/Model/GLTF/Tiles/1/Wall/Default/wall.gltf");

	// �ǁi�㕔�̂݁j
	wall_ceiling_mesh.Initialize(device, TileData::M_WallCeiling, tile_wall_ceiling_mesh_filepath, max_instance_count);
	wall_ceiling_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Wall::Default), "Data/Model/GLTF/Tiles/1/WallCeiling/Default/ceiling.gltf");

	// �o�^������񂩂�e���b�V���̃e�N�X�`���z����쐬����
	InitializeMaterials(device);
}

void TerrainRenderer::InitializeMaterials(ID3D11Device* device)
{
	// �e�N�X�`���z����쐬����ۂɃf�o�C�X�R���e�L�X�g�ɃA�N�Z�X����
	std::lock_guard<std::mutex> lock(MySystem.GetMutex());

	floor_mesh.CreateMaterialView(device, MySystem.GetDeviceContext());
	wall_mesh.CreateMaterialView(device, MySystem.GetDeviceContext());
	wall_ceiling_mesh.CreateMaterialView(device, MySystem.GetDeviceContext());
}

void TerrainRenderer::Render(
	ID3D11DeviceContext* dc, 
	Shader* shader, 
	const Terrain& terrain)
{
	floor_mesh.Render(dc, shader, terrain);
	wall_mesh.Render(dc, shader, terrain);
	wall_ceiling_mesh.Render(dc, shader, terrain);
}
