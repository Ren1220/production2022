#include "GLTFModelData.h"


#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#endif

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

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

/* --- ��` ---- */

GLTFModelData::GLTFModelData(ID3D11Device* device, const char* filepath)
	:filepath(filepath)
{
	// �ǂݍ��񂾃��f���f�[�^
	tinygltf::Model tinygltf_model_data;
	// GLTF���f����ǂݍ��ނ��߂̃N���X
	tinygltf::TinyGLTF tinygltf_model_loader;

	// �t�@�C���p�X���烂�f���f�[�^��ǂݍ���
	{
		std::string error, warning;
		bool is_succeded{ false };

		std::filesystem::path path{ filepath };
		// �t�@�C���̊g���q�� ".glb" �̏ꍇ
		if (path.extension() == ".glb")
		{
			// �o�C�i���f�[�^�ł̓ǂݍ���(�������̕�������)
			is_succeded = tinygltf_model_loader.LoadBinaryFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}
		// �t�@�C���̊g���q�� ".gltf" �̏ꍇ
		else if (path.extension() == ".gltf")
		{
			// json�t�@�C���ł̓ǂݍ���(�������̕����f�[�^���킩��₷��)
			is_succeded = tinygltf_model_loader.LoadASCIIFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}

		_ASSERT_EXPR_A(warning.empty(), warning.c_str());
		_ASSERT_EXPR_A(error.empty(), error.c_str());
		_ASSERT_EXPR_A(is_succeded, L"GLTF�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");

		// �t�@�C�������m�ۂ���
		filename = path.filename().string();
	}

	// �ǂݍ��񂾃��f���f�[�^����e�������o��
	FetchScenesFromModel(tinygltf_model_data);
	FetchNodesFromModel(tinygltf_model_data);
	FetchMeshesFromModel(device, tinygltf_model_data);
	FetchMaterialFromModel(device, tinygltf_model_data);
	FetchTexturesFromModel(device, tinygltf_model_data);
	FetchAnimationsFromModel(tinygltf_model_data);

	// �ėp�V�F�[�_�[�̍쐬�B
	// �ėp�V�F�[�_�[�̒��ŁA���̓��C�A�E�g���쐬����B
	// ���̓��C�A�E�g�͂��ׂẴ��f���`��V�F�[�_�[�ŋ��ʂɂ���B
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
	// �ǂݍ��񂾃��f���f�[�^
	tinygltf::Model tinygltf_model_data;
	// GLTF���f����ǂݍ��ނ��߂̃N���X
	tinygltf::TinyGLTF tinygltf_model_loader;


	auto out_animation{ std::make_shared<GLTFModelData::Animation>() };

	// �t�@�C���p�X���烂�f���f�[�^��ǂݍ���
	{
		std::string error, warning;
		bool is_succeded{ false };

		std::filesystem::path path{ filepath };
		// �t�@�C���̊g���q�� ".glb" �̏ꍇ
		if (path.extension() == ".glb")
		{
			// �o�C�i���f�[�^�ł̓ǂݍ���(�������̕�������)
			is_succeded = tinygltf_model_loader.LoadBinaryFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}
		// �t�@�C���̊g���q�� ".gltf" �̏ꍇ
		else if (path.extension() == ".gltf")
		{
			// json�t�@�C���ł̓ǂݍ���(�������̕����f�[�^���킩��₷��)
			is_succeded = tinygltf_model_loader.LoadASCIIFromFile(&tinygltf_model_data, &error, &warning, filepath);
		}

		_ASSERT_EXPR_A(warning.empty(), warning.c_str());
		_ASSERT_EXPR_A(error.empty(), error.c_str());
		_ASSERT_EXPR_A(is_succeded, L"GLTF�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");

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
				// sampler_inout_flag��true�̎���input,false�̎���output���Q�Ƃ���
				int32_t index{ (sampler_inout_flag) ?
					source_sampler.input : source_sampler.output };

				// index��input�̎��͔z��Ƀ^�C�����C�����
				//		  output�̎��͊e�L�[�t���[���ɂ�����m�[�h�̕��s�ړ��A�p���A�X�P�[�����O���i�[�����
				const auto& datas{
					data_buffer.emplace(index, source_accessor.count)
				};

				bool is_emplaced{ datas.second };

				if (is_emplaced)
				{
					const void* p_data{
						tinygltf_model_data.buffers.at(source_buffer_view.buffer).data.data()
						+ source_buffer_view.byteOffset + source_accessor.byteOffset };

					// �^�C�����C�������o�b�t�@������o��
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

				// �A�j���[�V�����f�[�^���X�P�[�����̏ꍇ
				if (source_channel.target_path == "scale")
				{
					fetch_animation_data(source_sampler, source_accessor, source_buffer_view, out_animation->scales);
				}
				// �A�j���[�V�����f�[�^����]���̏ꍇ
				else if (source_channel.target_path == "rotation")
				{
					fetch_animation_data(source_sampler, source_accessor, source_buffer_view, out_animation->rotations);
				}
				// �A�j���[�V�����f�[�^�����s�ړ����̏ꍇ
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

		// �ǂݍ��񂾃��f���f�[�^�ɃO���[�o����ԍs�񂪑��݂��Ă��Ȃ�
		if (!source_node.matrix.empty())
		{
			Matrix matrix{};

			// GLTF��OpenGL�n�Ȃ̂ōs��̒l�̈ʒu���قȂ邽�߁A
			// �ϊ����s���K�v������B
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

			// �ϊ������s��𕪉����āA���s�ړ��ʁA��]�ʁA�X�P�[�����O���擾����
			matrix.DecomposeTransform(node->translation, node->rotation, node->scale);

		}
		// �ǂݍ��񂾃��f���f�[�^�ɃO���[�o����ԍs�񂪑��݂��Ă���
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

	// �m�[�h�̃O���[�o���s����v�Z����
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
		// �����͊������邪�A����ɂ��K�v�ȃf�[�^�̐擪�A�h���X���擾�ł���
		// �ڍׂ�gltfOverView�Q��
		const void* p_subresource_data { 
			source_model.buffers.at(tinygltf_buffer_view.buffer).data.data() 
			+ tinygltf_buffer_view.byteOffset
			+ tinygltf_accessor.byteOffset };
		data.pSysMem = p_subresource_data;

		// ��Ŏg���̂Ő擪�A�h���X��Ԃ�
		*out_p_subresource_data = p_subresource_data;

		HRESULT hr = device->CreateBuffer(&desc, &data, buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	};

	for (const auto& source_mesh : source_model.meshes)
	{
		auto& mesh{ meshes.emplace_back() };

		mesh.name = source_mesh.name;

		// �C���f�b�N�X�o�b�t�@�A���_�o�b�t�@���擾���ACPU�AGPU�ň����`�Ɋe�X�ϊ�����
		for (const auto& source_primitive : source_mesh.primitives)
		{
			auto& primitive{ mesh.primitives.emplace_back() };

			primitive.material = source_primitive.material;

			// �C���f�b�N�X�o�b�t�@
			{
				auto& source_accessor{ source_model.accessors.at(source_primitive.indices) };
				auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

				primitive.index_buffer_view = MakeBufferView(source_accessor);

				// �C���f�b�N�X�o�b�t�@�f�[�^�̐擪�A�h���X
				const void* p_buffer_data{};

				// �C���f�b�N�X�o�b�t�@�̍쐬
				create_buffer(
					static_cast<UINT>(primitive.index_buffer_view.size_in_bytes),
					D3D11_BIND_INDEX_BUFFER,
					source_accessor,
					source_buffer_view,
					primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf(),
					&p_buffer_data);

				// �C���f�b�N�X�f�[�^��CPU�ň����`�ɕϊ����Ď擾����
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

			// ���_�o�b�t�@
			for (const auto& [attribute_name, accessor_index] : source_primitive.attributes)
			{
				const auto& source_accessor{ source_model.accessors.at(accessor_index) };
				const auto& source_buffer_view{ source_model.bufferViews.at(source_accessor.bufferView) };

				BufferView buffer_view{ MakeBufferView(source_accessor) };

				// ���_�o�b�t�@�f�[�^�̐擪�A�h���X
				const void* p_buffer_data{};

				// ���_�o�b�t�@�̍쐬
				create_buffer(
					static_cast<UINT>(buffer_view.size_in_bytes),
					D3D11_BIND_VERTEX_BUFFER,
					source_accessor,
					source_buffer_view,
					buffer_view.buffer.ReleaseAndGetAddressOf(),
					&p_buffer_data);

				// ���_�o�b�t�@�z��ɍ쐬�����f�[�^���i�[����
				primitive.vertex_buffer_views.emplace(std::make_pair(attribute_name, buffer_view));

				// ���_�A�@���̃f�[�^��CPU�ň����`�ɕϊ����Ď擾����
				if (attribute_name == "POSITION")
				{
					auto p_data{ reinterpret_cast<const Vector3*>(p_buffer_data) };

// 					for (size_t i = 0; i < buffer_view.Count(); ++i)
// 					{
// 						primitive.positions.emplace_back(p_data[i]);
// 					}
					std::span datas{ p_data, buffer_view.Count() };
					primitive.positions = std::vector<Vector3>(datas.begin(), datas.end());


					// ���_�̍ő�l�A�ŏ��l���擾����
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

					// ���������������ȁH(c++20)
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

			// �s�����Ă���f�[�^�����݂���ꍇ�_�~�[�f�[�^���i�[����
			const std::unordered_map<std::string, BufferView> attributes
			{
				{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
				{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
				{ "JOINTS_0", { DXGI_FORMAT_R8G8B8A8_UINT } },
				{ "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			};

			for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
			{
				// �f�[�^��������Ȃ������ꍇ�͑Ή�����_�~�[�f�[�^��ǉ�����
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
	// ���f������}�e���A���f�[�^���擾����
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

		// �F���
		material.data.pbr_metallic_roughness.basecolor_factor[0] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(0));
		material.data.pbr_metallic_roughness.basecolor_factor[1] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(1));
		material.data.pbr_metallic_roughness.basecolor_factor[2] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(2));
		material.data.pbr_metallic_roughness.basecolor_factor[3] =
				static_cast<float>(source_material.pbrMetallicRoughness.baseColorFactor.at(3));
		// �J���[�e�N�X�`�����
		material.data.pbr_metallic_roughness.basecolor_texture.index =
				source_material.pbrMetallicRoughness.baseColorTexture.index;
		material.data.pbr_metallic_roughness.basecolor_texture.texcoord =
				source_material.pbrMetallicRoughness.baseColorTexture.texCoord;
		// ���˗��p�����[�^
		material.data.pbr_metallic_roughness.metallic_factor =
				static_cast<float>(source_material.pbrMetallicRoughness.metallicFactor);
		// �e���p�����[�^
		material.data.pbr_metallic_roughness.roughness_factor =
				static_cast<float>(source_material.pbrMetallicRoughness.roughnessFactor);

		// PBR�e�N�X�`�����
		material.data.pbr_metallic_roughness.metallic_roughness_texture.index =
				source_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord =
				source_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

		// �@���e�N�X�`��
		material.data.normal_texture.index    = source_material.normalTexture.index;
		material.data.normal_texture.texcoord = source_material.normalTexture.texCoord;
		material.data.normal_texture.scale    = static_cast<float>(source_material.normalTexture.scale);

		// �A�e�e�N�X�`��
		material.data.occlusion_texture.index    = source_material.occlusionTexture.index;
		material.data.occlusion_texture.texcoord = source_material.occlusionTexture.texCoord;
		material.data.occlusion_texture.strength = static_cast<float>(source_material.occlusionTexture.strength);

		// ���Ȕ����e�N�X�`��
		material.data.emissive_texture.index    = source_material.emissiveTexture.index;
		material.data.emissive_texture.texcoord = source_material.emissiveTexture.texCoord;
	}

	// �擾�����}�e���A���f�[�^���܂Ƃ߂�GPU�ɑ��邽�߂̍\�����o�b�t�@���쐬���A
	// �V�F�[�_���\�[�X�r���[���爵����悤�ɂ���
	{
		std::vector<Material::StrBufferMaterial> material_subresources;

		for (const auto& material : materials)
		{
			material_subresources.emplace_back(material.data);
		}

		// �擾�������ׂẴ}�e���A���f�[�^����\�����o�b�t�@���쐬����
		if (!material_subresources.empty())
		{
			const UINT data_size{ sizeof(Material::StrBufferMaterial) };
			const UINT data_count{ static_cast<UINT>(material_subresources.size()) };

			HRESULT hr;

			// �\�����o�b�t�@�̍쐬
			Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
			D3D11_BUFFER_DESC buffer_desc{ };
			buffer_desc.ByteWidth		    = data_size * data_count;
			buffer_desc.StructureByteStride = data_size; 
			buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
			// �\�����o�b�t�@�̓V�F�[�_���\�[�X�r���[�ǂ��āAGPU�ɑ���
			buffer_desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE; 
			buffer_desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			// https://docs.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-cs-resources
			D3D11_SUBRESOURCE_DATA data{ };
			data.pSysMem = material_subresources.data();
			hr = device->CreateBuffer(&buffer_desc, &data, buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			
			// �V�F�[�_���\�[�X�r���[�̍쐬
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

	// �e�N�X�`���͑Ή�����摜�f�[�^�̃C���f�b�N�X���������Ă���
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

		// GLB�t�@�C���̏ꍇ
		if (source_image.bufferView > -1)
		{
			const auto& source_view{ source_model.bufferViews.at(source_image.bufferView) };
			const auto& source_buffer{ source_model.buffers.at(source_view.buffer) };
			const uint8_t* p_data{ source_buffer.data.data() + source_view.byteOffset };

			ID3D11ShaderResourceView* shader_resource_view{ };
			// �o�C�g��񂩂�e�N�X�`����ǂݍ���
			hr = ResourceFileLoader::LoadTexture(device, p_data, source_view.byteLength, &shader_resource_view);

			if (SUCCEEDED(hr))
			{
				// �z��Ƀ|�C���^��}������ƁA
				// shader_resource_view �̎Q�ƃJ�E���g���������
				// (ComPtr�z��Ȃ̂ŃR���X�g���N�^���������邱�ƂŎQ�ƃJ�E���g�����Z�����)
				// texture_resource_views.emplace_back(shader_resource_view); 

				// ���[�J���ϐ��ł��� shader_resource_view�͉������邪
				// ���|�C���^�Ȃ̂ŎQ�ƃJ�E���g���ω�����
				// ������Ȃ��ƎQ�ƃJ�E���g�����炸�A���������[�N���N����
				//shader_resource_view->Release(); 

				// Attach/Detach�͎Q�ƃJ�E���g�𑝂₳�Ȃ�
				texture_resource_views.emplace_back().Attach(shader_resource_view);
			}
		}
		// GLTF�t�@�C���̏ꍇ
		else
		{
			const std::filesystem::path path(filepath);
			D3D11_TEXTURE2D_DESC desc;

			// �e�N�X�`���̃t�@�C���p�X���擾���� �������炭�t�@�C���̏ꏊ�Ƀe�N�X�`�������݂��Ȃ��ꍇ�͓ǂݍ��܂�Ȃ�
			std::wstring w_texture_filepath{
				path.parent_path().concat(L"/").wstring() +
				std::wstring(source_image.uri.begin(), source_image.uri.end())
			};

			ID3D11ShaderResourceView* shader_resource_view{ };
			hr = ResourceFileLoader::LoadTexture(device, w_texture_filepath.c_str(), &shader_resource_view, &desc);

			if (hr == S_OK)
			{
				// �z��Ƀ|�C���^��}������ƁA
				// shader_resource_view �̎Q�ƃJ�E���g���������
				// (ComPtr�z��Ȃ̂ŃR���X�g���N�^���������邱�ƂŎQ�ƃJ�E���g�����Z�����)
				// texture_resource_views.emplace_back(shader_resource_view); 

				// ���[�J���ϐ��ł��� shader_resource_view�͉������邪
				// ���|�C���^�Ȃ̂ŎQ�ƃJ�E���g���ω�����
				// ������Ȃ��ƎQ�ƃJ�E���g�����炸�A���������[�N���N����
				// shader_resource_view->Release(); 

				// Attach/Detach�͎Q�ƃJ�E���g�𑝂₳�Ȃ�
				texture_resource_views.emplace_back().Attach(shader_resource_view);
			}

			// GLTF�t�@�C���̏ꍇ�́A�e�N�X�`���̃T�C�Y���͓ǂݍ��񂾃t�@�C������擾����K�v������
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


		// ���o���t�s��z��̃f�[�^�̐擪�A�h���X
		const void* p_source_data{ source_model.buffers.at(source_buffer_view.buffer).data.data() + source_buffer_view.byteOffset + source_accessor.byteOffset };
		size_t source_data_length{ source_accessor.count * sizeof(Matrix) };

		// �t�s��z��̃f�[�^�����o��
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
			// sampler_inout_flag��true�̎���input,false�̎���output���Q�Ƃ���
			int32_t index{ (sampler_inout_flag) ? 
				source_sampler.input : source_sampler.output };

			// index��input�̎��͔z��Ƀ^�C�����C�����
			//		  output�̎��͊e�L�[�t���[���ɂ�����m�[�h�̕��s�ړ��A�p���A�X�P�[�����O���i�[�����
			const auto& datas{
				data_buffer.emplace(index, source_accessor.count)
			};

			bool is_emplaced{ datas.second };

			if (is_emplaced)
			{
				const void* p_data{ 
					source_model.buffers.at(source_buffer_view.buffer).data.data() 
					+ source_buffer_view.byteOffset + source_accessor.byteOffset };

				// �^�C�����C�������o�b�t�@������o��
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

			// �A�j���[�V�����f�[�^���X�P�[�����̏ꍇ
			if (source_channel.target_path == "scale")
			{
				fetch_animation_data(source_sampler, source_accessor, source_buffer_view, animation.scales);
			}
			// �A�j���[�V�����f�[�^����]���̏ꍇ
			else if (source_channel.target_path == "rotation")
			{
				fetch_animation_data(source_sampler, source_accessor, source_buffer_view, animation.rotations);
			}
			// �A�j���[�V�����f�[�^�����s�ړ����̏ꍇ
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

	// StructureOfArray�ɂ�钸�_�o�b�t�@�̍\��
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
	// LIFO�R���e�i
	std::stack<Matrix> parent_global_transforms {};

	// �X�^�b�N�R���e�i�̓����𐶂������ƂŁA
	// �q�m�[�h���R���e�i������o���s�񂪐e�m�[�h�̃O���[�o���s��ƂȂ�
	// ����ɂ��A�ċA�������s�����ƂŁA���ׂẴm�[�h�̃O���[�o���s����v�Z����
	std::function<void(int)> traverse{
		[&](int node_index)-> void
		{
			std::shared_ptr<Node>& node{ nodes.at(node_index) };

			Matrix local_transform{ Matrix::CreateTransform(node->translation, node->rotation, node->scale) };

			// �O���[�o���s��̌v�Z
			node->global_transform = local_transform * parent_global_transforms.top();										

			for (int child_node_index : node->children)
			{
				// �X�^�b�N�Ɏ��g�̃O���[�o���s���ǉ�����
				parent_global_transforms.push(node->global_transform);
				// �ċA����
				traverse(child_node_index);
				// �O���[�o���s�����j������
				parent_global_transforms.pop();
			}
		}
	};

	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		// �P�ʍs��i���E�̒��S�j��ǉ�����
		parent_global_transforms.push(Matrix::Identity());
		// �O���[�o���s����쐬����
		traverse(node_index);
		// ��x���[���h�s���j������
		parent_global_transforms.pop();
	}
}

GLTFModelData::BufferView GLTFModelData::MakeBufferView(const tinygltf::Accessor& accessor)
{
	BufferView out_buffer_view;

	// �A�N�Z�T����o�b�t�@�̃f�[�^�t�H�[�}�b�g�𔻕ʂ���
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
			_ASSERT_EXPR(false, L"�T�|�[�g����Ă��Ȃ��f�[�^�T�C�Y�̃A�N�Z�T�ł�");
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
			_ASSERT_EXPR(false, L"�T�|�[�g����Ă��Ȃ��f�[�^�T�C�Y�̃A�N�Z�T�ł�");
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
			_ASSERT_EXPR(false, L"�T�|�[�g����Ă��Ȃ��f�[�^�T�C�Y�̃A�N�Z�T�ł�");
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
			_ASSERT_EXPR(false, L"�T�|�[�g����Ă��Ȃ��f�[�^�T�C�Y�̃A�N�Z�T�ł�");
			break;
		}
		break;
	}
	default:
		_ASSERT_EXPR(false, L"�T�|�[�g����Ă��Ȃ���ނ̃A�N�Z�T�ł�");
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
