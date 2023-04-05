#pragma once

/* --- 概要 ---- */

// 地形データから取得した情報をもとに、地形のインスタンス描画を行う。
// マテリアルはベースとなるモデルから作成したものから抽出した物を利用する。
// 初期化時にテクスチャ配列を作成することで、描画するマテリアルを切り替える。

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <filesystem>

#include "Graphics/Shader/Shader.h"

#include "Terrain.h"
#include "TerrainMesh.h"

/* --- 宣言 ---- */

class TerrainRenderer
{
public:

	TerrainRenderer() = default;
	~TerrainRenderer() = default;

	void Initialize(
		ID3D11Device* device, 
		uint32_t max_instance_count = 10000U);

	/// <summary>
	/// 各メッシュのインスタンス描画を行う、
	/// マテリアルは対応するマップのタイルから設定される。
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="shader"></param>
	/// <param name="terrain"></param>
	void Render(ID3D11DeviceContext* dc, Shader* shader, const Terrain& terrain);

	const TerrainMesh& GetFloorMesh() const { return floor_mesh; }
	const TerrainMesh& GetWallMesh() const { return wall_mesh; }

private:

	void InitializeMaterials(ID3D11Device* device);

	// インスタンス描画を行うメッシュモデルのファイルパス
	static constexpr const char* tile_floor_mesh_filepath       { "Data/Model/GLTF/Tiles/_Base/Floor/Default/floor.gltf" };
	static constexpr const char* tile_wall_mesh_filepath        { "Data/Model/GLTF/Tiles/_Base/Wall/Default/wall.gltf" };
	static constexpr const char* tile_wall_ceiling_mesh_filepath{ "Data/Model/GLTF/Tiles/_Base/WallCeiling/Default/ceiling.gltf" };

	TerrainMesh floor_mesh;
	TerrainMesh wall_mesh;
	TerrainMesh wall_ceiling_mesh;

};
