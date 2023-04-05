#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>


#include "Graphics/Model/GLTFModel.h"
#include "Graphics/Resource/StructuredBuffer.h"

#include "Terrain.h"
#include "TileData.h"

/* --- 宣言 ---- */

class TerrainMesh
{
public:

	TerrainMesh()
		: max_instance_count(), material_datas(), mesh_id(TileData::MeshID::M_None) {}
	virtual ~TerrainMesh() = default;

	void Initialize(ID3D11Device* device, TileData::MeshID id, const char* filepath, uint32_t max_instance_count);

	void Render(ID3D11DeviceContext* dc, Shader* shader, const Terrain& terrain);

	/// <summary>
	/// インスタンス描画用マテリアルとして引数のファイルパスのモデルデータからマテリアル情報を取り出す。
	/// 初期化時に作成したモデルと同一のメッシュにエディタ(Blender)でマテリアルを設定し、それを出力したものを
	/// 利用する。
	/// <para>注) テクスチャのサイズは統一しておく</para>
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath">マテリアル情報を取り出すモデルデータ</param>
	void AddMaterial(
		ID3D11Device* device,
		size_t material_index,
		const char* filepath,
		const wchar_t* displacement_texture = nullptr,
		float displacement_scale = 1.0f);

	/// <summary>
	/// <para>vector配列に登録したマテリアル情報を元にマテリアル情報バッファ、各テクスチャの配列を作成し、MaterialViewで参照する。</para>
	/// 各テクスチャのインデックスは追加した順番になっている。
	/// <para>注) テクスチャ配列ではテクスチャのサイズを統一しないと都合が悪いためテクスチャのサイズを引数の値に統一すること。</para>
	/// </summary>
	void CreateMaterialView(
		ID3D11Device* device, 
		ID3D11DeviceContext* dc,
		uint32_t texture_size = 256U);

	const GLTFModel* GetModel()	const { return model.get(); }

private:

	struct InstanceStructuredBuffer
	{
		Matrix world;
		uint32_t material_index;
		int is_displacement;
		float displacement_scale;
	};

	uint32_t max_instance_count;
	TileData::MeshID mesh_id;

	std::unique_ptr<GLTFModel> model;

	StructuredBuffer<InstanceStructuredBuffer> instance_structued_buffer;
	StructuredBuffer<GLTFModelData::Material::StrBufferMaterial> material_structured_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> basecolor_texture_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness_metallic_texture_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal_texture_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissive_texture_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> occlusion_texture_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> displacement_texture_view;

	struct MaterialData
	{
		std::shared_ptr<GLTFModelData> material;

		struct Displacement
		{
			std::wstring filepath;
			float scale;
		} displacement;
	};

	std::vector<MaterialData> material_datas;
};
