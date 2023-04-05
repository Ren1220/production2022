#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>


#include "Graphics/Model/GLTFModel.h"
#include "Graphics/Resource/StructuredBuffer.h"

#include "Terrain.h"
#include "TileData.h"

/* --- �錾 ---- */

class TerrainMesh
{
public:

	TerrainMesh()
		: max_instance_count(), material_datas(), mesh_id(TileData::MeshID::M_None) {}
	virtual ~TerrainMesh() = default;

	void Initialize(ID3D11Device* device, TileData::MeshID id, const char* filepath, uint32_t max_instance_count);

	void Render(ID3D11DeviceContext* dc, Shader* shader, const Terrain& terrain);

	/// <summary>
	/// �C���X�^���X�`��p�}�e���A���Ƃ��Ĉ����̃t�@�C���p�X�̃��f���f�[�^����}�e���A���������o���B
	/// ���������ɍ쐬�������f���Ɠ���̃��b�V���ɃG�f�B�^(Blender)�Ń}�e���A����ݒ肵�A������o�͂������̂�
	/// ���p����B
	/// <para>��) �e�N�X�`���̃T�C�Y�͓��ꂵ�Ă���</para>
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath">�}�e���A���������o�����f���f�[�^</param>
	void AddMaterial(
		ID3D11Device* device,
		size_t material_index,
		const char* filepath,
		const wchar_t* displacement_texture = nullptr,
		float displacement_scale = 1.0f);

	/// <summary>
	/// <para>vector�z��ɓo�^�����}�e���A���������Ƀ}�e���A�����o�b�t�@�A�e�e�N�X�`���̔z����쐬���AMaterialView�ŎQ�Ƃ���B</para>
	/// �e�e�N�X�`���̃C���f�b�N�X�͒ǉ��������ԂɂȂ��Ă���B
	/// <para>��) �e�N�X�`���z��ł̓e�N�X�`���̃T�C�Y�𓝈ꂵ�Ȃ��Ɠs�����������߃e�N�X�`���̃T�C�Y�������̒l�ɓ��ꂷ�邱�ƁB</para>
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
