#include "TerrainRenderer.h"

/* --- 追加のヘッダファイルのインクルード ---- */


#include "General/AtaiEngine.h"

/* --- 定義 ---- */

void TerrainRenderer::Initialize(ID3D11Device* device, uint32_t max_instance_count)
{
	// 床
	floor_mesh.Initialize(device, TileData::M_Floor, tile_floor_mesh_filepath, max_instance_count);
	floor_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Floor::Default), "Data/Model/GLTF/Tiles/1/Floor/Default/floor.gltf");
	floor_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Floor::Warp), "Data/Model/GLTF/Tiles/1/Floor/Warp/floor.gltf");

	// 壁
	wall_mesh.Initialize(device, TileData::M_Wall, tile_wall_mesh_filepath, max_instance_count);
	wall_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Wall::Default), "Data/Model/GLTF/Tiles/1/Wall/Default/wall.gltf");

	// 壁（上部のみ）
	wall_ceiling_mesh.Initialize(device, TileData::M_WallCeiling, tile_wall_ceiling_mesh_filepath, max_instance_count);
	wall_ceiling_mesh.AddMaterial(device, static_cast<size_t>(TileMaterial::Wall::Default), "Data/Model/GLTF/Tiles/1/WallCeiling/Default/ceiling.gltf");

	// 登録した情報から各メッシュのテクスチャ配列を作成する
	InitializeMaterials(device);
}

void TerrainRenderer::InitializeMaterials(ID3D11Device* device)
{
	// テクスチャ配列を作成する際にデバイスコンテキストにアクセスする
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
