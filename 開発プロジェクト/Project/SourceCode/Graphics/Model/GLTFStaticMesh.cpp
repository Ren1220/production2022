#include "GLTFStaticMesh.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/Resource/ConstantBufferManager.h"

/* --- ��` ---- */

GLTFStaticMesh::GLTFStaticMesh(ID3D11Device* device, const char* filepath)
	: GLTFModel(device, filepath)
{
	// �萔�o�b�t�@�̍쐬
	primitive_cb.Create(device);
}

GLTFStaticMesh::~GLTFStaticMesh()
{
}

void GLTFStaticMesh::Render(ID3D11DeviceContext* dc, Shader* shader) const
{
	Render(dc, shader, GetModelData()->material_resource_view.Get());
}

void GLTFStaticMesh::Render(
	ID3D11DeviceContext* dc,
	Shader* shader,
	ID3D11ShaderResourceView* material_structured_buffer_view) const
{
	auto model_data{ GetModelData() };

	if (shader)
	{
		shader->Activate(dc);
	}
	else
	{
		// �V�F�[�_��ݒ肵�Ȃ��ꍇ�͔ėp�V�F�[�_�[�𗘗p����
		model_data->ActivateDefaultShader(dc);
	}

	// ���̓��C�A�E�g�͂��ׂẴ��f���`��V�F�[�_�[�ŋ���
	dc->IASetInputLayout(model_data->GetInputLayout().Get());

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �s�N�Z���V�F�[�_�[�̐ݒ�
	dc->PSSetShaderResources(0, 1, &material_structured_buffer_view);

	for (const auto& node_index : model_data->scenes.at(0).nodes)
	{
		// �m�[�h���ƂɃm�[�h�̃��b�V����`�悷��ċA�֐�
		DrawMeshRecursive(dc, node_index);
	}
}

void GLTFStaticMesh::DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const
{
	// ���f���f�[�^�̎Q�Ƃ̎擾
	auto model_data{ GetModelData() };
	// �C���f�b�N�X�̃m�[�h�̎Q��
	const std::shared_ptr<GLTFModelData::Node>& node{ model_data->nodes.at(node_index) };

	// ���b�V�������݂���ꍇ
	if (node->mesh > -1)
	{
		const GLTFModelData::Mesh& mesh{ model_data->meshes.at(node->mesh) };

		// �v���~�e�B�u�P�ʂł̕`��
		for (std::vector<GLTFModelData::Mesh::Primitive>::const_reference primitive : mesh.primitives)
		{
			// ���_�o�b�t�@�̐ݒ�
			{
				// ���_�V�F�[�_�[��SoA�Ȃ̂ŁA����ɍ��킹���`�Œ��_�o�b�t�@���쐬����
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

			// �v���~�e�B�u�萔�o�b�t�@�̐ݒ�
			{
				CBufferPrimitive cb{ };
				const auto& world_transform{ GetWorldTransform() };

				cb.material = primitive.material;
				cb.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != nullptr;

				cb.world = node->global_transform * world_transform;

				primitive_cb.UpdateSubresouce(dc, cb);
				Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferPrimitive::slot, &primitive_cb);
			}

			// �e�N�X�`�����̐ݒ�
			{
				if (primitive.material > -1)
				{
					const GLTFModelData::Material& material{ model_data->materials.at(primitive.material) };

					// �e�N�X�`���C���f�b�N�X�z��
					const int texture_indices[]
					{
						material.data.pbr_metallic_roughness.basecolor_texture.index,         // �J���[
						material.data.pbr_metallic_roughness.metallic_roughness_texture.index,// �e���Ɣ��˗�
						material.data.normal_texture.index,									  // �@��
						material.data.emissive_texture.index,								  // ���g����̔���
						material.data.occlusion_texture.index,								  // �A�e
					};

					ID3D11ShaderResourceView* null_shader_resource_view{ };
					// �e�N�X�`���C���f�b�N�X�z��̑傫���̔z����쐬����
					std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

					for (int32_t texture_index = 0; texture_index < shader_resource_views.size(); ++
						texture_index)
					{
						// �v���~�e�B�u�ɔԍ��̎�ނ̃e�N�X�`�����ݒ肳��Ă���ꍇ�͏���n���A�ݒ肳��Ă��Ȃ��ꍇ�͋�̏���n���B
						shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
							model_data->texture_resource_views.
							at(model_data->textures.
								at(texture_indices[texture_index]).
								source).Get() :
							null_shader_resource_view;
					}

					// �s�N�Z���V�F�[�_�[�Ƀe�N�X�`������n��
					dc->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()),
						shader_resource_views.data());
				}
			}

			// �`����s
			dc->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.Count()), 0, 0);
		}
	}

	// �q�m�[�h�����݂���ꍇ
	for (std::vector<int32_t>::value_type child_index : node->children)
	{
		DrawMeshRecursive(dc, child_index);
	}
}
