#include "TerrainMesh.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <filesystem>
#include <WICTextureLoader.h>
#include <functional>

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Model/GLTFModelResource.h"

/* --- 定義 ---- */

void TerrainMesh::Initialize(
	ID3D11Device* device, 
	TileData::MeshID id,
	const char* filepath, 
	uint32_t max_instance_count)
{
	this->max_instance_count = max_instance_count;
	mesh_id = id;

	model = std::make_unique<GLTFStaticMesh>(device, filepath);

	// メッシュの種類に応じてマテリアル配列のサイズを決定する。
	// インスタンス描画用のマテリアル配列を作る際に不足している部分は黒テクスチャ{0,0,0,0}が割り当てられる。
	switch (id)
	{
	case TileData::M_None:
		_ASSERTE(0);
		break;
	case TileData::M_Floor:
		material_datas.resize(static_cast<size_t>(TileMaterial::Floor::ElementCount));
		break;
	case TileData::M_Wall:
	case TileData::M_WallCeiling:
		material_datas.resize(static_cast<size_t>(TileMaterial::Wall::ElementCount));
		break;
	}

	// 各タイルの情報をシェーダーに送るバッファ
	instance_structued_buffer.Create(device, nullptr, max_instance_count);
}

void TerrainMesh::Render(ID3D11DeviceContext* dc, Shader* shader, const Terrain& terrain)
{
	// モデルデータの参照の取得
	auto model_data{ model->GetModelData() };

	shader->Activate(dc);
	//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(model_data->GetInputLayout().Get());

	const int32_t node_index{ model_data->scenes.at(0).nodes.at(0) };
	const auto& node{ model_data->nodes.at(node_index) };
	const auto& mesh{ model_data->meshes.at(node->mesh) };
	const auto& primitive{ mesh.primitives.at(0) }; // 描画するデータ


	// プリミティブ単位での描画
	// 頂点バッファの設定
	{
		// 頂点シェーダーがSoAなので、それに合わせた形で頂点バッファを作成する
		ID3D11Buffer* vertex_buffers[]
		{
			primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
			primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
			primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
			primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
			primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
			primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
		};

		UINT strides[]
		{
			static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
			static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
			static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
			static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
			static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
			static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
		};

		UINT offsets[_countof(vertex_buffers)]{ 0 };

		dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
		dc->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
			primitive.index_buffer_view.format, 0);
	}

	// プリミティブ、マテリアル、テクスチャ配列の情報をGPUに送る
	{
		uint32_t draw_count{};
		// プリミティブ情報の更新
		{
			const auto& map{ terrain.GetMap() };

			std::vector<InstanceStructuredBuffer> datas(max_instance_count);


			for (uint32_t ri = 0U; ri < terrain.GetMapHeight(); ++ri)
			{
				for (uint32_t ci = 0U; ci < terrain.GetMapWidth(); ++ci)
				{
					const auto& tile{ map[ci][ri] };

					if (tile.data.mesh != mesh_id) continue;

					//描画を行うタイルを追加していく

					// 床、壁によって対応したインデックスに変換される
					datas[draw_count].material_index = static_cast<uint32_t>(tile.material.index);
					datas[draw_count].world = terrain.CalcWorldTransform(ci, ri);
					datas[draw_count].is_displacement = !material_datas[tile.material.index].displacement.filepath.empty();
					datas[draw_count].displacement_scale = material_datas[tile.material.index].displacement.scale;

					// 描画するインスタンスの数をカウントする。
					++draw_count;
				}
			}

			instance_structued_buffer.UpdateSubresource(dc, datas.data(), datas.size());
		}

		// 各情報のビュー配列
		std::vector<ID3D11ShaderResourceView*> shader_resource_views
		{
			instance_structued_buffer.GetView().Get(),
			material_structured_buffer.GetView().Get(),
			basecolor_texture_view.Get(),
			roughness_metallic_texture_view.Get(),
			normal_texture_view.Get(),
			emissive_texture_view.Get(),
			occlusion_texture_view.Get(),
			displacement_texture_view.Get()
		};

		// シェーダーに情報を渡す
		dc->PSSetShaderResources(
			0,
			static_cast<UINT>(shader_resource_views.size()),
			shader_resource_views.data());

		dc->VSSetShaderResources(
			0,
			static_cast<UINT>(shader_resource_views.size()),
			shader_resource_views.data());

		dc->DSSetShaderResources(
			0,
			static_cast<UINT>(shader_resource_views.size()),
			shader_resource_views.data());

		dc->DrawIndexedInstanced(
			static_cast<UINT>(primitive.index_buffer_view.Count()),
			draw_count,
			0, 0, 0);
	}
}

void TerrainMesh::AddMaterial(
	ID3D11Device* device,
	size_t material_index,
	const char* filepath,
	const wchar_t* displacement_texture,
	float displacement_scale)
{
	material_datas[material_index].material = GLTFModelResource::GetInstance().LoadModelData(device, filepath);
	material_datas[material_index].displacement = { displacement_texture != nullptr ? displacement_texture : std::wstring(), displacement_scale };
}

void TerrainMesh::CreateMaterialView(ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t texture_size)
{
	HRESULT hr{ S_OK };
	// マテリアルを抽出する各モデルデータのマテリアル配列の先頭データのマテリアルから、
	// PBR描画に用いる各テクスチャを取り出し、テクスチャ配列を作成する。

	const uint32_t material_count{ static_cast<uint32_t>(material_datas.size()) };

	std::vector<GLTFModelData::Material::StrBufferMaterial> material_temp_datas;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> basecolor_texture_temp_data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> roughness_metallic_texture_temp_data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> normal_texture_temp_data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> emissive_texture_temp_data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> occlsion_texture_temp_data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> displacement_texture_temp_data;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> dummy_texture;
	{
		HRESULT hr{ S_OK };

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = desc.Height = texture_size;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;			 // 読み書き専用
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // 読み書き専用
		desc.BindFlags = 0;

		// ピクセルサイズを計算する
		size_t texels{ texture_size * texture_size };
		// 各ピクセルにカラーコードの情報を入力する
		std::vector<DWORD> system(texels, 0x00000000);

		size_t offset = 0;

		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = system.data();
		subresource_data.SysMemPitch = sizeof(DWORD) * texture_size;

		hr = device->CreateTexture2D(&desc, &subresource_data, dummy_texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// in_texture_viewのテクスチャ情報を、out_texture_array_viewに書き込んでいく
	auto _write_to_texture_array_view =
		[&](UINT i,
			const GLTFModelData* model_data,
			DXGI_FORMAT format,  // テクスチャによってカラーフォーマットが異なる
			ID3D11Texture2D** texture2d_array_temp_data,
			int32_t ref_texture_index,
			ID3D11ShaderResourceView** out_texture_array_view)
	{
		UINT mip_levels{ 1 };

		if (i == 0)
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = format;
			desc.Width = desc.Height = texture_size;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.ArraySize = material_count;
			desc.MipLevels = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			hr = device->CreateTexture2D(&desc, NULL, texture2d_array_temp_data);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_resource;

		if (ref_texture_index > -1)
		{
			// const auto& texture_view{ model_data->texture_resource_views.at(ref_texture_index) };
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//texture_view->GetResource(resource.GetAddressOf());
			//hr = resource->QueryInterface<ID3D11Texture2D>(texture_resource.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			const auto& image{ model_data->images.at(model_data->textures.at(ref_texture_index).source) };
			const std::filesystem::path path(model_data->filepath);

			// テクスチャのファイルパスを取得する ※おそらくファイルの場所にテクスチャが存在しない場合は読み込まれない
			std::wstring w_texture_filepath{
				path.parent_path().concat(L"/").wstring() +
				std::wstring(image.uri.begin(), image.uri.end())
			};

			hr = DirectX::CreateWICTextureFromFileEx(
				device,
				w_texture_filepath.c_str(),
				0ULL,
				D3D11_USAGE_STAGING,
				0,
				D3D11_CPU_ACCESS_READ,
				0,
				DirectX::WIC_LOADER_FORCE_RGBA32,
				resource.GetAddressOf(),
				nullptr);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			hr = resource->QueryInterface<ID3D11Texture2D>(texture_resource.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			texture_resource = dummy_texture;
		}


		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		hr = dc->Map(texture_resource.Get(), NULL, D3D11_MAP_READ, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 空テクスチャーのサブリソースをファイルから
		// 画像ファイルを読み込んで作成したテクスチャーの内容で更新する
		for (UINT iMip = 0; iMip < mip_levels; iMip++)
		{
			dc->UpdateSubresource(*texture2d_array_temp_data,
				D3D11CalcSubresource(iMip, i, mip_levels),
				NULL,
				mapped_subresource.pData,
				mapped_subresource.RowPitch,
				0
			);
		}
		dc->Unmap(texture_resource.Get(), NULL);

	};

	auto write_to_texture_array_view_from_path =
		[&](UINT i,
			const std::wstring& filepath,
			DXGI_FORMAT format,  // テクスチャによってカラーフォーマットが異なる
			ID3D11Texture2D** texture2d_array_temp_data,
			ID3D11ShaderResourceView** out_texture_array_view)
	{
		UINT mip_levels{ 1 };

		if (i == 0)
		{
			D3D11_TEXTURE2D_DESC desc{};
			desc.Format = format;
			desc.Width = desc.Height = texture_size;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.ArraySize = material_count;
			desc.MipLevels = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			hr = device->CreateTexture2D(&desc, NULL, texture2d_array_temp_data);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_resource;

		if (!filepath.empty())
		{
			// const auto& texture_view{ model_data->texture_resource_views.at(ref_texture_index) };
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//texture_view->GetResource(resource.GetAddressOf());
			//hr = resource->QueryInterface<ID3D11Texture2D>(texture_resource.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			hr = DirectX::CreateWICTextureFromFileEx(
				device,
				filepath.c_str(),
				0ULL,
				D3D11_USAGE_STAGING,
				0,
				D3D11_CPU_ACCESS_READ,
				0,
				DirectX::WIC_LOADER_FORCE_RGBA32,
				resource.GetAddressOf(),
				nullptr);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			hr = resource->QueryInterface<ID3D11Texture2D>(texture_resource.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			texture_resource = dummy_texture;
		}


		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		hr = dc->Map(texture_resource.Get(), NULL, D3D11_MAP_READ, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 空テクスチャーのサブリソースをファイルから
		// 画像ファイルを読み込んで作成したテクスチャーの内容で更新する
		for (UINT iMip = 0; iMip < mip_levels; iMip++)
		{
			dc->UpdateSubresource(*texture2d_array_temp_data,
				D3D11CalcSubresource(iMip, i, mip_levels),
				NULL,
				mapped_subresource.pData,
				mapped_subresource.RowPitch,
				0
			);
		}
		dc->Unmap(texture_resource.Get(), NULL);

	};

	// 各テクスチャ配列を作成する
	for (UINT texture_array_slice_offset = 0; const auto & material_data : material_datas)
	{

		// テクスチャはすべてR8G8B8A8フォーマットにする

		const auto& material{ material_data.material->materials.at(0) };

		material_temp_datas.emplace_back(material.data);

		auto write_to_texture_array_view = std::bind(
			_write_to_texture_array_view,
			texture_array_slice_offset,
			material_data.material.get(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4);

		write_to_texture_array_view(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			basecolor_texture_temp_data.GetAddressOf(),
			material.data.pbr_metallic_roughness.basecolor_texture.index,
			basecolor_texture_view.GetAddressOf());

		write_to_texture_array_view(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			roughness_metallic_texture_temp_data.GetAddressOf(),
			material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
			roughness_metallic_texture_view.GetAddressOf());

		write_to_texture_array_view(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			normal_texture_temp_data.GetAddressOf(),
			material.data.normal_texture.index,
			normal_texture_view.GetAddressOf());

		write_to_texture_array_view(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			emissive_texture_temp_data.GetAddressOf(),
			material.data.emissive_texture.index,
			emissive_texture_view.GetAddressOf());

		write_to_texture_array_view(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			occlsion_texture_temp_data.GetAddressOf(),
			material.data.occlusion_texture.index,
			occlusion_texture_view.GetAddressOf());

		write_to_texture_array_view_from_path(
			texture_array_slice_offset,
			material_data.displacement.filepath,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			displacement_texture_temp_data.GetAddressOf(),
			displacement_texture_view.GetAddressOf());

		++texture_array_slice_offset;
	}

	material_structured_buffer.Create(device, material_temp_datas.data(), material_temp_datas.size());

	// 作成したテクスチャ配列バッファを参照するシェーダーリソースビューを作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // テクスチャ配列をビューするときの設定
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = material_count; // 配列のテクスチャの数
	hr = device->CreateShaderResourceView(basecolor_texture_temp_data.Get(), &desc, basecolor_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateShaderResourceView(roughness_metallic_texture_temp_data.Get(), &desc, roughness_metallic_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateShaderResourceView(normal_texture_temp_data.Get(), &desc, normal_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateShaderResourceView(emissive_texture_temp_data.Get(), &desc, emissive_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateShaderResourceView(occlsion_texture_temp_data.Get(), &desc, occlusion_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateShaderResourceView(displacement_texture_temp_data.Get(), &desc, displacement_texture_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
