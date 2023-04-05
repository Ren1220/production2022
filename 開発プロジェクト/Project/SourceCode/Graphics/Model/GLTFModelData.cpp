#include "GLTFModelData.h"


#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#endif

/* --- 追加のヘッダファイルのインクルード ---- */

#include <filesystem>
#include <stack>
#include <algorithm>
#include <ranges>
#include <span>
#include <filesystem>

#include "imgui.h"

#include "Include/tinygltf-master/tiny_gltf.h"
#include "Include/misc.h"

#include "../General/ResourceFileloader.h"

/* --- 定義 ---- */

GLTFModelData::GLTFModelData(ID3D11Device* device, const char* filepath)
	:filepath(filepath)
{
	// 読み込んだモデルデータ
	tinygltf::Model tinygltf_model_data;
	// GLTFモデルを読み込むためのクラス
	tinygltf::TinyGLTF tinygltf_model_loader;

	// ファイルパスからモデルデータを読み込む
	{
		std::string error, warning;
		bool is_succeded{ false };

		std::filesystem::path path{ filepath };
		// ファイルの拡張子が ".glb" の場合
		if (path.extension() == ".glb")
		{
			// バイナリデータでの読み込み(こっちの方が速い)
			is_succeded = tinygltf_model_loader.LoadBinaryFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}
		// ファイルの拡張子が ".gltf" の場合
		else if (path.extension() == ".gltf")
		{
			// jsonファイルでの読み込み(こっちの方がデータがわかりやすい)
			is_succeded = tinygltf_model_loader.LoadASCIIFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}

		_ASSERT_EXPR_A(warning.empty(), warning.c_str());
		_ASSERT_EXPR_A(error.empty(), error.c_str());
		_ASSERT_EXPR_A(is_succeded, L"GLTFファイルの読み込みに失敗しました");

		// ファイル名を確保する
		filename = path.filename().string();
	}

	// 読み込んだモデルデータから各情報を取り出す
	FetchScenesFromModel(tinygltf_model_data);
	FetchNodesFromModel(tinygltf_model_data);
	FetchMeshesFromModel(device, tinygltf_model_data);
	FetchMaterialFromModel(device, tinygltf_model_data);
	FetchTexturesFromModel(device, tinygltf_model_data);
	FetchAnimationsFromModel(tinygltf_model_data);

	// 汎用シェーダーの作成。
	// 汎用シェーダーの中で、入力レイアウトも作成する。
	// 入力レイアウトはすべてのモデル描画シェーダーで共通にする。
	{
		auto element_desc{ CreateInputLayoutDesc() };

		default_shader.CreateVSFromCSO(
			device, 
			defalut_shader_path_vs, 
			input_layout.ReleaseAndGetAddressOf(),
			element_desc.data(),
			static_cast<UINT>(element_desc.size()));

		default_shader.CreatePSFromCSO(device, defalut_shader_path_ps);
	}

}

std::shared_ptr<GLTFModelData::Animation> GLTFModelData::LoadAnimation(const char* filepath) 
{
	// 読み込んだモデルデータ
	tinygltf::Model tinygltf_model_data;
	// GLTFモデルを読み込むためのクラス
	tinygltf::TinyGLTF tinygltf_model_loader;


	auto out_animation{ std::make_shared<GLTFModelData::Animation>() };

	// ファイルパスからモデルデータを読み込む
	{
		std::string error, warning;
		bool is_succeded{ false };

		std::filesystem::path path{ filepath };
		// ファイルの拡張子が ".glb" の場合
		if (path.extension() == ".glb")
		{
			// バイナリデータでの読み込み(こっちの方が速い)
			is_succeded = tinygltf_model_loader.LoadBinaryFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}
		// ファイルの拡張子が ".gltf" の場合
		else if (path.extension() == ".gltf")
		{
			// jsonファイルでの読み込み(こっちの方がデータがわかりやすい)
			is_succeded = tinygltf_model_loader.LoadASCIIFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}

		_ASSERT_EXPR_A(warning.empty(), warning.c_str());
		_ASSERT_EXPR_A(error.empty(), error.c_str());
		_ASSERT_EXPR_A(is_succeded, L"GLTFファイルの読み込みに失敗しました");

		out_animation->name = path.string();
	}


	{
		for (const auto& source_animation : tinygltf_model_data.animations)
		{
			using namespace std::placeholders;


			auto fetch_data = [&tinygltf_model_data]<class T>(
				bool sampler_inout_flag,
				const tinygltf::AnimationSampler & source_sampler,
				const tinygltf::Accessor & source_accessor,
				const tinygltf::BufferView & source_buffer_view,
				std::unordered_map<int32_t, std::vector<T>>&data_buffer
				) -> void
			{
				// sampler_inout_flagがtrueの時はinput,falseの時はoutputを参照する
				int32_t index{ (sampler_inout_flag) ?
					source_sampler.input : source_sampler.output };

				// indexがinputの時は配列にタイムライン情報
				//		  outputの時は各キーフレームにおけるノードの平行移動、姿勢、スケーリングが格納される
				const auto& datas{
					data_buffer.emplace(index, source_accessor.count)
				};

				bool is_emplaced{ datas.second };

				if (is_emplaced)
				{
					const void* p_data{
						tinygltf_model_data.buffers.at(source_buffer_view.buffer).data.data()
						+ source_buffer_view.byteOffset + source_accessor.byteOffset };

					// タイムライン情報をバッファから取り出す
					memcpy(datas.first->second.data(), p_data, source_accessor.count * sizeof(T));
				}

			};

			constexpr bool sampler_input_flag{ true };
			constexpr bool sampler_output_flag{ false };

			// 
			auto fetch_timeline = std::bind(fetch_data, sampler_input_flag, _1, _2, _3, _4);
			auto fetch_animation_data = std::bind(fetch_data, sampler_output_flag, _1, _2, _3, _4);

			for (const auto& source_sampler : source_animation.samplers)
			{
				auto& sampler{ out_animation->samplers.emplace_back() };
				sampler.input = source_sampler.input;
				sampler.output = source_sampler.output;
				sampler.interpolation = source_sampler.interpolation;

				const auto& source_accessor{ tinygltf_model_data.accessors.at(source_sampler.input) };
				const auto& source_buffer_view{ tinygltf_model_data.bufferViews.at(source_accessor.bufferView) };

				fetch_timeline(source_sampler, source_accessor, source_buffer_view, out_animation->timelines);
			}

			for (const auto& source_channel : source_animation.channels)
			{
				auto& channel{ out_animation->channels.emplace_back() };
				channel.sampler = source_channel.sampler;
				channel.target_node = source_channel.target_node;
				channel.target_path = source_channel.target_path;

				const auto& source_sampler{ source_animation.samplers.at(source_channel.sampler) };
				const auto& source_accessor{ tinygltf_model_data.accessors.at(source_sampler.output) };
				const auto& source_buffer_view{ tinygltf_model_data.bufferViews.at(source_accessor.bufferView) };

				// アニメーションデータがスケール情報の場合
				if (source_channel.target_path == "scale")
				{
					fetch_animation_data(source_sampler, source_accessor, source_buffer_view, out_animation->scales);
				}
				// アニメーションデータが回転情報の場合
				else if (source_channel.target_path == "rotation")
				{
					fetch_animation_data(source_sampler, source_accessor, source_buffer_view, out_animation->rotations);
				}
				// アニメーションデータが平行移動情報の場合
				else if (source_channel.target_path == "translation")
				{
					fetch_animation_data(source_sampler, source_accessor, source_buffer_view, out_animation->translations);
				}
			}
		}
	}

	return out_animation;
}

void GLTFModelData::ActivateDefaultShader(ID3D11DeviceContext* dc)
{
	default_shader.Activate(dc);
}

void GLTFModelData::FetchScenesFromModel(const tinygltf::Model& source_model)
{
	for (const auto& source_scene : source_model.scenes)
	{
		Scene& scene{ scenes.emplace_back() };

		scene.name = source_scene.name;
		scene.nodes = source_scene.nodes;
	}
}

void GLTFModelData::FetchNodesFromModel(const tinygltf::Model& source_model)
{
	for (const auto& source_node : source_model.nodes)
	{
		auto& node{ nodes.emplace_back() };

		node = std::make_shared<Node>();
		node->name     = source_node.name;
		node->skin     = source_node.skin;
		node->mesh     = source_node.mesh;
		node->children = source_node.children;

		// 読み込んだモデルデータにグローバル空間行列が存在していない
		if (!source_node.matrix.empty())
		{
			Matrix matrix{};

			// GLTFはOpenGL系なので行列の値の位置が異なるため、
			// 変換を行う必要がある。
			// M(GL) = | _11, _21, _31, _41 |
			//		   | _12, _22, _32, _42 |
			//		   | _13, _23, _33, _43 |
			//		   | _14, _24, _34, _44 |

			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(source_node.matrix.at(4 * row + column));
				}
			}

			// 変換した行列を分解して、平行移動量、回転量、スケーリングを取得する
			matrix.DecomposeTransform(node->translation, node->rotation, node->scale);

		}
		// 読み込んだモデルデータにグローバル空間行列が存在している
		else
		{
			if (source_node.scale.size() > 0)
			{
				node->scale.x = static_cast<float>(source_node.scale.at(0));
				node->scale.y = static_cast<float>(source_node.scale.at(1));
				node->scale.z = static_cast<float>(source_node.scale.at(2));
			}
			if (source_node.translation.size() > 0)
			{
				node->translation.x = static_cast<float>(source_node.translation.at(0));
				node->translation.y = static_cast<float>(source_node.translation.at(1));
				node->translation.z = static_cast<float>(source_node.translation.at(2));
			}
			if (source_node.rotation.size() > 0)
			{
				node->rotation.x = static_cast<float>(source_node.rotation.at(0));
				node->rotation.y = static_cast<float>(source_node.rotation.at(1));
				node->rotation.z = static_cast<float>(source_node.rotation.at(2));
				node->rotation.w = static_cast<float>(source_node.rotation.at(3));
			}
		}
	}

	// ノードのグローバル行列を計算する
	CumulateTransforms(nodes);
}

void GLTFModelData::FetchMeshesFromModel(ID3D11Device* device, const tinygltf::Model& source_model)
{
	HRESULT hr{ S_OK };

	auto create_buffer = [&device, source_model](
		UINT byte_width,
		D3D11_BIND_FLAG bind_flag, 
		const tinygltf::Accessor& tinygltf_accessor, 
		const tinygltf::BufferView& tinygltf_buffer_view,
		ID3D11Buffer** buffer,
		const void** out_p_subresource_data)
		-> void
	{
		D3D11_BUFFER_DESC desc{ };
		desc.ByteWidth = byte_width;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = bind_flag;

		D3D11_SUBRESOURCE_DATA data{ };
		// 説明は割愛するが、これにより必要なデータの先頭アドレスが取得できる
		// 詳細はgltfOverView参照
		const void* p_subresource_data { 
			source_model.buffers.at(tinygltf_buffer_view.buffer).data.data() 
			+ tinygltf_buffer_view.byteOffset
			+ tinygltf_accessor.byteOffset };
		data.pSysMem = p_subresource_data;

		// 後で使うので先頭アドレスを返す
		*out_p_subresource_data = p_subresource_data;

		HRESULT hr = device->CreateBuffer(&desc, &data, buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	};

	for (const auto& source_mesh : source_model.meshes)
	{
		auto& mesh{ meshes.emplace_back() };

		mesh.name = source_mesh.name;

		// インデックスバッファ、頂点バッファを取得し、CPU、GPUで扱う形に各々変換する
		for (const auto& source_primitive : source_mesh.primitives)
		{
			auto& primitive{ mesh.primitives.emplace_back() };

			primitive.material = source_primitive.material;

			// インデックスバッファ
			{
				auto& source_accessor{ source_model.accessors.at(source_primitive.indices) };
				auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

				primitive.index_buffer_view = MakeBufferView(source_accessor);

				// インデックスバッファデータの先頭アドレス
				const void* p_buffer_data{};

				// インデックスバッファの作成
				create_buffer(
					static_cast<UINT>(primitive.index_buffer_view.size_in_bytes),
					D3D11_BIND_INDEX_BUFFER,
					source_accessor,
					source_buffer_view,
					primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf(),
					&p_buffer_data);

				// インデックスデータをCPUで扱う形に変換して取得する
				{
					const uint16_t* p_data{ reinterpret_cast<const uint16_t*>(p_buffer_data) };

					//for (size_t i = 0; i < primitive.index_buffer_view.Count(); ++i)
					//{
					//	primitive.indices.emplace_back(static_cast<uint16_t>(p_data[i]));
					//}

					std::span datas{ p_data, primitive.index_buffer_view.Count() };
					primitive.indices = std::vector<uint16_t>(datas.begin(), datas.end());
				}
			}

			// 頂点バッファ
			for (const auto& [attribute_name, accessor_index] : source_primitive.attributes)
			{
				const auto& source_accessor{ source_model.accessors.at(accessor_index) };
				const auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

				BufferView buffer_view{ MakeBufferView(source_accessor) };

				// 頂点バッファデータの先頭アドレス
				const void* p_buffer_data{};

				// 頂点バッファの作成
				create_buffer(
					static_cast<UINT>(buffer_view.size_in_bytes),
					D3D11_BIND_VERTEX_BUFFER,
					source_accessor,
					source_buffer_view,
					buffer_view.buffer.ReleaseAndGetAddressOf(),
					&p_buffer_data);

				// 頂点バッファ配列に作成したデータを格納する
				primitive.vertex_buffer_views.emplace(std::make_pair(attribute_name, buffer_view));

				// 頂点、法線のデータをCPUで扱う形に変換して取得する
				if (attribute_name == "POSITION")
				{
					auto p_data{ reinterpret_cast<const Vector3*>(p_buffer_data) };

// 					for (size_t i = 0; i < buffer_view.Count(); ++i)
// 					{
// 						primitive.positions.emplace_back(p_data[i]);
// 					}
					std::span datas{ p_data, buffer_view.Count() };
					primitive.positions = std::vector<Vector3>(datas.begin(), datas.end());


					// 頂点の最大値、最小値を取得する
					{
						primitive.min.x = static_cast<float>(source_accessor.minValues.at(0));
						primitive.min.y = static_cast<float>(source_accessor.minValues.at(1));
						primitive.min.z = static_cast<float>(source_accessor.minValues.at(2));
						primitive.max.x = static_cast<float>(source_accessor.maxValues.at(0));
						primitive.max.y = static_cast<float>(source_accessor.maxValues.at(1));
						primitive.max.z = static_cast<float>(source_accessor.maxValues.at(2));
					}

				}
				else if (attribute_name == "NORMAL")
				{
					auto p_data{ reinterpret_cast<const Vector3*>(p_buffer_data) };

					//for (size_t i = 0; i < buffer_view.Count(); ++i)
					//{
					//	primitive.normals.emplace_back(data_begin[i]);
					//}

					// こっちが早いかな？(c++20)
					std::span datas{ p_data, buffer_view.Count() };
					primitive.normals = std::vector<Vector3>(datas.begin(), datas.end());

				}
				//else if (gltf_attribute.first == "JOINTS_0")
				//{
				//	auto data_begin{ reinterpret_cast<const uint8_t*>(gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset) };
				//
				//	for(size_t i = 0; i < buffer_view.Count(); i+=4)
				//	{
				//		primitive.joints.emplace_back(
				//			data_begin[i],data_begin[i+1],data_begin[i+2],data_begin[i+3]);
				//	}
				//}
				//else if(gltf_attribute.first == "WEIGHTS_0")
				//{
				//	auto data_begin{ reinterpret_cast<const Vector4*>(gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset) };
				//
				//	for (size_t i = 0; i < buffer_view.Count(); ++i)
				//	{
				//		primitive.weights.emplace_back(data_begin[i]);
				//	}
				//}
			}

			// 不足しているデータが存在する場合ダミーデータを格納する
			const std::unordered_map<std::string, BufferView> attributes
			{
				{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
				{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
				{ "JOINTS_0", { DXGI_FORMAT_R8G8B8A8_UINT } },
				{ "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			};

			for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
			{
				// データが見つからなかった場合は対応するダミーデータを追加する
				if (!primitive.vertex_buffer_views.contains(attribute.first))
				{
					primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
				}
			}
		}
	}
}

void GLTFModelData::FetchMaterialFromModel(ID3D11Device* device, const tinygltf::Model& source_model)
{
	// モデルからマテリアルデータを取得する
	for (const auto& source_material : source_model.materials)
	{
		auto& material{ materials.emplace_back() };

		material.name = source_material.name;

		material.data.emissive_factor[0] = static_cast<float>(source_material.emissiveFactor.at(0));
		material.data.emissive_factor[1] = static_cast<float>(source_material.emissiveFactor.at(1));
		material.data.emissive_factor[2] = static_cast<float>(source_material.emissiveFactor.at(2));

		// https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AlphaBlendModeTest/README.md
		if	   (source_material.alphaMode == "OPAQUE") material.data.alpha_mode = 0;
		else if(source_material.alphaMode == "MASK"  ) material.data.alpha_mode = 1;
		else if(source_material.alphaMode == "BLEND" ) material.data.alpha_mode = 2;

		material.data.alpha_cutoff = static_cast<float>(source_material.alphaCutoff);
		material.data.double_sided = source_material.doubleSided ? 1 : 0;

		// 色情報
		material.data.pbr_metallic_roughness.basecolor_factor[0] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(0));
		material.data.pbr_metallic_roughness.basecolor_factor[1] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(1));
		material.data.pbr_metallic_roughness.basecolor_factor[2] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(2));
		material.data.pbr_metallic_roughness.basecolor_factor[3] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(3));
		// カラーテクスチャ情報
		material.data.pbr_metallic_roughness.basecolor_texture.index =
				source_material.pbrMetallicRoughness.baseColorTexture.index;
		material.data.pbr_metallic_roughness.basecolor_texture.texcoord =
				source_material.pbrMetallicRoughness.baseColorTexture.texCoord;
		// 反射率パラメータ
		material.data.pbr_metallic_roughness.metallic_factor =
				static_cast<float>(source_material.pbrMetallicRoughness.metallicFactor);
		// 粗さパラメータ
		material.data.pbr_metallic_roughness.roughness_factor =
				static_cast<float>(source_material.pbrMetallicRoughness.roughnessFactor);

		// PBRテクスチャ情報
		material.data.pbr_metallic_roughness.metallic_roughness_texture.index =
				source_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord =
				source_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

		// 法線テクスチャ
		material.data.normal_texture.index    = source_material.normalTexture.index;
		material.data.normal_texture.texcoord = source_material.normalTexture.texCoord;
		material.data.normal_texture.scale    = static_cast<float>(source_material.normalTexture.scale);

		// 陰影テクスチャ
		material.data.occlusion_texture.index    = source_material.occlusionTexture.index;
		material.data.occlusion_texture.texcoord = source_material.occlusionTexture.texCoord;
		material.data.occlusion_texture.strength = static_cast<float>(source_material.occlusionTexture.strength);

		// 自己発光テクスチャ
		material.data.emissive_texture.index    = source_material.emissiveTexture.index;
		material.data.emissive_texture.texcoord = source_material.emissiveTexture.texCoord;
	}

	// 取得したマテリアルデータをまとめてGPUに送るための構造化バッファを作成し、
	// シェーダリソースビューから扱えるようにする
	{
		std::vector<Material::StrBufferMaterial> material_subresources;

		for (const auto& material : materials)
		{
			material_subresources.emplace_back(material.data);
		}

		// 取得したすべてのマテリアルデータから構造化バッファを作成する
		if (!material_subresources.empty())
		{
			const UINT data_size{ sizeof(Material::StrBufferMaterial) };
			const UINT data_count{ static_cast<UINT>(material_subresources.size()) };

			HRESULT hr;

			// 構造化バッファの作成
			Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
			D3D11_BUFFER_DESC buffer_desc{ };
			buffer_desc.ByteWidth		    = data_size * data_count;
			buffer_desc.StructureByteStride = data_size; 
			buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
			// 構造化バッファはシェーダリソースビューどして、GPUに送る
			buffer_desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE; 
			buffer_desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			// https://docs.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-cs-resources
			D3D11_SUBRESOURCE_DATA data{ };
			data.pSysMem = material_subresources.data();
			hr = device->CreateBuffer(&buffer_desc, &data, buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			
			// シェーダリソースビューの作成
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{ };
			shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
			shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			shader_resource_view_desc.Buffer.NumElements = data_count;
			hr = device->CreateShaderResourceView(buffer.Get(), &shader_resource_view_desc,
			                                      material_resource_view.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
}

void GLTFModelData::FetchTexturesFromModel(ID3D11Device* device, const tinygltf::Model& source_model)
{
	HRESULT hr;

	// テクスチャは対応する画像データのインデックスを所持している
	for (const auto& source_texture : source_model.textures)
	{
		auto& texture{ textures.emplace_back() };
		texture.name   = source_texture.name;
		texture.source = source_texture.source;
	}

	for (const auto& source_image : source_model.images)
	{
		auto& image{ images.emplace_back() };

		image.name        = source_image.name;
		image.width       = source_image.width;
		image.height      = source_image.height;
		image.component   = source_image.component;
		image.bits        = source_image.bits;
		image.pixel_type  = source_image.pixel_type;
		image.buffer_view = source_image.bufferView;
		image.mime_type   = source_image.mimeType;
		image.uri         = source_image.uri;
		image.as_is       = source_image.as_is;

		// GLBファイルの場合
		if (source_image.bufferView > -1)
		{
			const auto& source_view{ source_model.bufferViews.at(source_image.bufferView) };
			const auto& source_buffer{ source_model.buffers.at(source_view.buffer) };
			const uint8_t* p_data{ source_buffer.data.data() + source_view.byteOffset };

			ID3D11ShaderResourceView* shader_resource_view{ };
			// バイト情報からテクスチャを読み込む
			hr = ResourceFileLoader::LoadTexture(device, p_data, source_view.byteLength, &shader_resource_view);

			if (SUCCEEDED(hr))
			{
				// 配列にポインタを挿入すると、
				// shader_resource_view の参照カウントが一つ増える
				// (ComPtr配列なのでコンストラクタが発生することで参照カウントが加算される)
				// texture_resource_views.emplace_back(shader_resource_view); 

				// ローカル変数である shader_resource_viewは解放されるが
				// 生ポインタなので参照カウントが変化せず
				// 解放しないと参照カウントが減らず、メモリリークを起こす
				//shader_resource_view->Release(); 

				// Attach/Detachは参照カウントを増やさない
				texture_resource_views.emplace_back().Attach(shader_resource_view);
			}
		}
		// GLTFファイルの場合
		else
		{
			const std::filesystem::path path(filepath);
			D3D11_TEXTURE2D_DESC desc;

			// テクスチャのファイルパスを取得する ※おそらくファイルの場所にテクスチャが存在しない場合は読み込まれない
			std::wstring w_texture_filepath{
				path.parent_path().concat(L"/").wstring() +
				std::wstring(source_image.uri.begin(), source_image.uri.end())
			};

			ID3D11ShaderResourceView* shader_resource_view{ };
			hr = ResourceFileLoader::LoadTexture(device, w_texture_filepath.c_str(), &shader_resource_view, &desc);

			if (hr == S_OK)
			{
				// 配列にポインタを挿入すると、
				// shader_resource_view の参照カウントが一つ増える
				// (ComPtr配列なのでコンストラクタが発生することで参照カウントが加算される)
				// texture_resource_views.emplace_back(shader_resource_view); 

				// ローカル変数である shader_resource_viewは解放されるが
				// 生ポインタなので参照カウントが変化せず
				// 解放しないと参照カウントが減らず、メモリリークを起こす
				// shader_resource_view->Release(); 

				// Attach/Detachは参照カウントを増やさない
				texture_resource_views.emplace_back().Attach(shader_resource_view);
			}

			// GLTFファイルの場合は、テクスチャのサイズ等は読み込んだファイルから取得する必要がある
			image.width  = static_cast<int32_t>(desc.Width);
			image.height = static_cast<int32_t>(desc.Height);
		}
	}
}

void GLTFModelData::FetchAnimationsFromModel(const tinygltf::Model& source_model)
{
	using namespace std;

	for (const auto& transmission_skin : source_model.skins)
	{
		auto& skin{ skins.emplace_back() };

		const auto& source_accessor{ source_model.accessors.at(transmission_skin.inverseBindMatrices) };
		const auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };


		// 取り出す逆行列配列のデータの先頭アドレス
		const void* p_source_data{ source_model.buffers.at(source_buffer_view.buffer).data.data() + source_buffer_view.byteOffset + source_accessor.byteOffset };
		size_t source_data_length{ source_accessor.count * sizeof(Matrix) };

		// 逆行列配列のデータを取り出す
		skin.inverse_bind_matrices.resize(source_accessor.count);
		memcpy(skin.inverse_bind_matrices.data(), p_source_data, source_data_length);

		skin.joints = transmission_skin.joints;
	}

	for (const auto& source_animation : source_model.animations)
	{
		using namespace std::placeholders;

		auto& animation{ animations.emplace_back() };

		animation.name = source_animation.name;

		auto fetch_data = [&source_model]<class T>(
			bool sampler_inout_flag,
			const tinygltf::AnimationSampler & source_sampler,
			const tinygltf::Accessor & source_accessor,
			const tinygltf::BufferView & source_buffer_view,
			std::unordered_map<int32_t, std::vector<T>>&data_buffer
			) -> void
		{
			// sampler_inout_flagがtrueの時はinput,falseの時はoutputを参照する
			int32_t index{ (sampler_inout_flag) ? 
				source_sampler.input : source_sampler.output };

			// indexがinputの時は配列にタイムライン情報
			//		  outputの時は各キーフレームにおけるノードの平行移動、姿勢、スケーリングが格納される
			const auto& datas{
				data_buffer.emplace(index, source_accessor.count)
			};

			bool is_emplaced{ datas.second };

			if (is_emplaced)
			{
				const void* p_data{ 
					source_model.buffers.at(source_buffer_view.buffer).data.data() 
					+ source_buffer_view.byteOffset + source_accessor.byteOffset };

				// タイムライン情報をバッファから取り出す
				memcpy(datas.first->second.data(), p_data, source_accessor.count * sizeof(T));
			}

		};

		constexpr bool sampler_input_flag{ true };
		constexpr bool sampler_output_flag{ false };

		// 
		auto fetch_timeline = std::bind(fetch_data, sampler_input_flag, _1, _2, _3, _4);
		auto fetch_animation_data = std::bind(fetch_data, sampler_output_flag, _1, _2, _3, _4);

		for (const auto& source_sampler : source_animation.samplers)
		{
			auto& sampler{ animation.samplers.emplace_back() };
			sampler.input         = source_sampler.input;
			sampler.output        = source_sampler.output;
			sampler.interpolation = source_sampler.interpolation;
			
			const auto& source_accessor{ source_model.accessors.at(source_sampler.input) };
			const auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

			fetch_timeline(source_sampler, source_accessor, source_buffer_view, animation.timelines);
		}

		for (const auto& source_channel : source_animation.channels)
		{
			auto& channel{ animation.channels.emplace_back() };
			channel.sampler     = source_channel.sampler;
			channel.target_node = source_channel.target_node;
			channel.target_path = source_channel.target_path;

			const auto& source_sampler{ source_animation.samplers.at(source_channel.sampler) };
			const auto& source_accessor{ source_model.accessors.at(source_sampler.output) };
			const auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

			// アニメーションデータがスケール情報の場合
			if (source_channel.target_path == "scale")
			{
				fetch_animation_data(source_sampler, source_accessor, source_buffer_view, animation.scales);
			}
			// アニメーションデータが回転情報の場合
			else if (source_channel.target_path == "rotation")
			{
				fetch_animation_data(source_sampler, source_accessor, source_buffer_view, animation.rotations);
			}
			// アニメーションデータが平行移動情報の場合
			else if (source_channel.target_path == "translation")
			{
				fetch_animation_data(source_sampler, source_accessor, source_buffer_view, animation.translations);
			}
		}
	}
}

std::array<D3D11_INPUT_ELEMENT_DESC, 6> GLTFModelData::CreateInputLayoutDesc()
{
	const auto& vertex_buffer_views{ meshes.at(0).primitives.at(0).vertex_buffer_views };

	// StructureOfArrayによる頂点バッファの構成
	// https://qiita.com/emadurandal/items/0bb83b545670475f51a3
	std::array<D3D11_INPUT_ELEMENT_DESC, 6> desc;
	
	desc[0] = { "POSITION",	 0, vertex_buffer_views.at("POSITION").format,	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	desc[1] = { "NORMAL",	 0, vertex_buffer_views.at("NORMAL").format,	1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	desc[2] = { "TANGENT",	 0, vertex_buffer_views.at("TANGENT").format,	2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	desc[3] = { "TEXCOORDS", 0, vertex_buffer_views.at("TEXCOORD_0").format,3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	desc[4] = { "JOINTS",	 0, vertex_buffer_views.at("JOINTS_0").format,	4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	desc[5] = { "WEIGHTS",	 0, vertex_buffer_views.at("WEIGHTS_0").format,	5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	
	return desc;
}

void GLTFModelData::CumulateTransforms(std::vector<std::shared_ptr<Node>>& nodes) const
{
	// LIFOコンテナ
	std::stack<Matrix> parent_global_transforms {};

	// スタックコンテナの特性を生かすことで、
	// 子ノードがコンテナから取り出す行列が親ノードのグローバル行列となる
	// これにより、再帰処理を行うことで、すべてのノードのグローバル行列を計算する
	std::function<void(int)> traverse{
		[&](int node_index)-> void
		{
			std::shared_ptr<Node>& node{ nodes.at(node_index) };

			Matrix local_transform{ Matrix::CreateTransform(node->translation, node->rotation, node->scale) };

			// グローバル行列の計算
			node->global_transform = local_transform * parent_global_transforms.top();										

			for (int child_node_index : node->children)
			{
				// スタックに自身のグローバル行列を追加する
				parent_global_transforms.push(node->global_transform);
				// 再帰処理
				traverse(child_node_index);
				// グローバル行列を一つ破棄する
				parent_global_transforms.pop();
			}
		}
	};

	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		// 単位行列（世界の中心）を追加する
		parent_global_transforms.push(Matrix::Identity());
		// グローバル行列を作成する
		traverse(node_index);
		// 一度ワールド行列を破棄する
		parent_global_transforms.pop();
	}
}

GLTFModelData::BufferView GLTFModelData::MakeBufferView(const tinygltf::Accessor& accessor)
{
	BufferView out_buffer_view;

	// アクセサからバッファのデータフォーマットを判別する
	switch (accessor.type)
	{
	case TINYGLTF_TYPE_SCALAR:
	{
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			out_buffer_view.format = DXGI_FORMAT_R16_UINT;
			out_buffer_view.stride_in_bytes = sizeof(USHORT);
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			out_buffer_view.format = DXGI_FORMAT_R32_UINT;
			out_buffer_view.stride_in_bytes = sizeof(UINT);
			break;
		default:
			_ASSERT_EXPR(false, L"サポートされていないデータサイズのアクセサです");
			break;
		}
		break;
	}
	case TINYGLTF_TYPE_VEC2: // float2
	{
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			out_buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
			out_buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
			break;
		default:
			_ASSERT_EXPR(false, L"サポートされていないデータサイズのアクセサです");
			break;
		}
		break;
	}
	case TINYGLTF_TYPE_VEC3: // float3
	{
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			out_buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
			out_buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
			break;
		default:
			_ASSERT_EXPR(false, L"サポートされていないデータサイズのアクセサです");
			break;
		}
		break;
	}
	case TINYGLTF_TYPE_VEC4: // float4
	{
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			out_buffer_view.format = DXGI_FORMAT_R8G8B8A8_UINT;
			out_buffer_view.stride_in_bytes = sizeof(BYTE) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			out_buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
			out_buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			out_buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
			out_buffer_view.stride_in_bytes = sizeof(UINT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			out_buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			out_buffer_view.stride_in_bytes = sizeof(FLOAT) * 4;
			break;
		default:
			_ASSERT_EXPR(false, L"サポートされていないデータサイズのアクセサです");
			break;
		}
		break;
	}
	default:
		_ASSERT_EXPR(false, L"サポートされていない種類のアクセサです");
		break;
	}

	out_buffer_view.size_in_bytes = (accessor.count * out_buffer_view.stride_in_bytes);

	return out_buffer_view;
}

float GLTFModelData::Animation::MeasureAnimationTimeEnd() const
{
	float end_time{ 0.0f };

	for (const auto& timeline : timelines | std::ranges::views::values)
	{
		float timeline_end_time{ *(timeline.end() - 1) };
		end_time = (std::max)(end_time, timeline_end_time);
	}

	return end_time;
}
