#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>


#include "Arithmetic/Arithmetic.h"

#include "../Shader/Shader.h"

/* --- �O���錾 ---- */

class GLTFModel;

namespace tinygltf
{
	class Model;
	struct Accessor;
}

/* --- �錾 ---- */

class GLTFModelData
{
	friend class GLTFModel;

public:


	struct Scene
	{
		std::string name;
		std::vector<int32_t> nodes; // ���[�g�m�[�h�z��
	};

	struct Node
	{
		std::string name;
		int32_t skin{ -1 }; // �X�L���C���f�b�N�X
		int32_t mesh{ -1 }; // ���b�V���C���f�b�N�X

		std::vector<int32_t> children; // �q�m�[�h�̃C���f�b�N�X�z��

		Quaternion rotation{ 0.0f,0.0f,0.0f,1.0f };
		Vector3 scale{ 1.0f,1.0f,1.0f };
		Vector3 translation{ 0.0f,0.0f,0.0f };

		Matrix global_transform{

			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f };
	};

	struct BufferView
	{
		DXGI_FORMAT format{ DXGI_FORMAT_UNKNOWN }; // �o�b�t�@�̃T�C�Y�̎��
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		size_t stride_in_bytes{ 0 }; // �o�b�t�@�̋�؂�T�C�Y
		size_t size_in_bytes{ 0 };	// �o�b�t�@�̃T�C�Y
		/// <summary>
		/// �f�[�^�̐���Ԃ�
		/// </summary>
		/// <returns></returns>
		size_t Count() const { return size_in_bytes / stride_in_bytes; }
	};

	struct Mesh
	{
		std::string name;
		struct Primitive
		{
			int32_t material{ -1 }; // �}�e���A���C���f�b�N�X
			std::map<std::string, BufferView> vertex_buffer_views; // ���_�o�b�t�@�r���[�z��
			BufferView index_buffer_view;

			std::vector<uint16_t> indices;
			std::vector<Vector3> positions;
			std::vector<Vector3> normals;
			//std::vector<DirectX::XMUINT4> joints;
			//std::vector<Vector4> weights;

			Vector3 min, max;
		};

		std::vector<Primitive> primitives;
	};

	struct TextureInfo
	{
		int32_t index{ -1 };
		int32_t texcoord{ 0 };
	};

	struct NormalTextureInfo
	{
		int32_t index{ -1 };
		int32_t texcoord{ 0 };
		float scale{ 1.0f };
	};

	struct OcclusionTextureInfo
	{
		int32_t index{ -1 };
		int32_t texcoord{ 0 };
		float strength{ 1.0f };
	};

	struct PBRMetallicRoughness
	{
		float basecolor_factor[4]{ 1.0f,1.0f,1.0f,1.0f };
		TextureInfo basecolor_texture;
		float metallic_factor = 1.0f;
		float roughness_factor = 1.0f;
		TextureInfo metallic_roughness_texture;
	};

	struct Material
	{
		std::string name;

		struct StrBufferMaterial
		{
			float emissive_factor[3]{ 0.0f,0.0f,0.0f };
			int32_t alpha_mode{ 0 }; //"OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
			float alpha_cutoff{ 0.5f };
			bool double_sided{ false };

			PBRMetallicRoughness pbr_metallic_roughness; // �e���Ɣ��˗�

			NormalTextureInfo normal_texture;			// �@��
			OcclusionTextureInfo occlusion_texture;		// ���g����̔���
			TextureInfo emissive_texture;				// �A�e
		};

		StrBufferMaterial data;
	};

	struct Texture
	{
		std::string name;
		int32_t source{ -1 };
	};

	struct Image
	{
		std::string name;
		int32_t width{ -1 };
		int32_t height{ -1 };
		int32_t component{ -1 };
		int32_t bits{ -1 };
		int32_t pixel_type{ -1 };
		
		int32_t buffer_view;	// .glb  �t�@�C���̏ꍇ�͂�����̏�񂪋L�ڂ���Ă���
		std::string mime_type;	// .glb  �t�@�C���̏ꍇ�͂�����̏�񂪋L�ڂ���Ă���
		std::string uri;		// .gltf �t�@�C���̏ꍇ�͂�����̏�񂪋L�ڂ���Ă���
		bool as_is{ false };
	};

	struct Skin
	{
		std::vector<Matrix> inverse_bind_matrices; // �t�s��z��
		std::vector<int32_t> joints;
	};

	struct Animation
	{
		std::string name;

		struct Channel
		{
			int32_t sampler{ -1 };
			int32_t target_node{ -1 };
			std::string target_path;
		};
		std::vector<Channel> channels;

		struct Sampler
		{
			int32_t input{ -1 };
			int32_t output{ -1 };
			std::string interpolation;
		};
		std::vector<Sampler> samplers;

		std::unordered_map<int32_t, std::vector<float>> timelines;
		std::unordered_map<int32_t, std::vector<Vector3>> scales;
		std::unordered_map<int32_t, std::vector<Quaternion>> rotations;
		std::unordered_map<int32_t, std::vector<Vector3>> translations;

		/// <summary>
		/// �A�j���[�V�����̏I�����Ԃ��v������
		/// (�A�j���V�������Ԃ͈̔͂͂����ȉ�)
		/// </summary>
		/// <returns></returns>
		float MeasureAnimationTimeEnd() const;

	};
public:

	GLTFModelData(ID3D11Device* device, const char* filepath);

	static std::shared_ptr<GLTFModelData::Animation> LoadAnimation(const char* filepath) ;

	const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetInputLayout() const { return input_layout; }

	void ActivateDefaultShader(ID3D11DeviceContext* dc);

private:

	// tinygltf�N���X�œǂݍ��񂾃��f����񂩂�e�f�[�^�����o��

	void FetchScenesFromModel(const tinygltf::Model& source_model);
	void FetchNodesFromModel(const tinygltf::Model& source_model);
	void FetchMeshesFromModel(ID3D11Device* device, const tinygltf::Model& source_model);
	void FetchMaterialFromModel(ID3D11Device* device, const tinygltf::Model& source_model);
	void FetchTexturesFromModel(ID3D11Device* device, const tinygltf::Model& source_model);
	void FetchAnimationsFromModel(const tinygltf::Model& source_model);

	/// <summary>
	/// ���f���̓��̓��C�A�E�g�͂��ׂẴV�F�[�_�[�ŋ���
	/// </summary>
	std::array<D3D11_INPUT_ELEMENT_DESC, 6> CreateInputLayoutDesc();

	/// <summary>
	/// �o�b�t�@�r���[�̍쐬
	/// </summary>
	/// <param name="accessor">�쐬����o�b�t�@�̃f�[�^��t�H�[�}�b�g�𔻕ʂ���</param>
	/// <returns></returns>
	BufferView MakeBufferView(const tinygltf::Accessor& accessor);

public:

	/// <summary>
	/// ���ׂẴm�[�h�̃O���[�o���s����v�Z����
	/// </summary>
	void CumulateTransforms(std::vector<std::shared_ptr<Node>>& nodes) const;

public:

	std::string filepath; // �t�@�C���p�X
	std::string filename; // �t�@�C����

	std::vector<Scene> scenes;
	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;
	std::vector<Texture> textures;
	std::vector<Image> images;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;
	std::vector<Skin> skins;
	std::vector<Animation> animations;

	static constexpr const char* defalut_shader_path_vs{ "Shader/GLTFModelVS.cso" };
	static constexpr const char* defalut_shader_path_ps{ "Shader/GLTFModelPS.cso" };
	//static constexpr const char* defalut_shader_path_vs{ "D:/GitHub/MyLibrary/DirectX3D/Shader/GLTFModelVS.cso" };
	//static constexpr const char* defalut_shader_path_ps{ "D:/GitHub/MyLibrary/DirectX3D/Shader/GLTFModelPS.cso" };

	Shader default_shader;
	// ���f���̓��̓V�O�l�`���͂��ׂẴV�F�[�_�[�ŋ��ʂ̕����g���ׁA
	// ���߂ɔėp�V�F�[�_�[���쐬���A���̃V�F�[�_�[�ł��������̓��C�A�E�g���g���B
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
