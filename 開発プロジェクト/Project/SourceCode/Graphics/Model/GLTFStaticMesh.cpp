#include "GLTFStaticMesh.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/Resource/ConstantBufferManager.h"

/* --- 定義 ---- */

GLTFStaticMesh::GLTFStaticMesh(ID3D11Device* device, const char* filepath)
	: GLTFModel(device, filepath)
{
	// 定数バッファの作成
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
		// シェーダを設定しない場合は汎用シェーダーを利用する
		model_data->ActivateDefaultShader(dc);
	}

	// 入力レイアウトはすべてのモデル描画シェーダーで共通
	dc->IASetInputLayout(model_data->GetInputLayout().Get());

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ピクセルシェーダーの設定
	dc->PSSetShaderResources(0, 1, &material_structured_buffer_view);

	for (const auto& node_index : model_data->scenes.at(0).nodes)
	{
		// ノードごとにノードのメッシュを描画する再帰関数
		DrawMeshRecursive(dc, node_index);
	}
}

void GLTFStaticMesh::DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const
{
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };
	// インデックスのノードの参照
	const std::shared_ptr<GLTFModelData::Node>& node{ model_data->nodes.at(node_index) };

	// メッシュが存在する場合
	if (node->mesh > -1)
	{
		const GLTFModelData::Mesh& mesh{ model_data->meshes.at(node->mesh) };

		// プリミティブ単位での描画
		for (std::vector<GLTFModelData::Mesh::Primitive>::const_reference primitive : mesh.primitives)
		{
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

			// プリミティブ定数バッファの設定
			{
				CBufferPrimitive cb{ };
				const auto& world_transform{ GetWorldTransform() };

				cb.material = primitive.material;
				cb.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != nullptr;

				cb.world = node->global_transform * world_transform;

				primitive_cb.UpdateSubresouce(dc, cb);
				Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferPrimitive::slot, &primitive_cb);
			}

			// テクスチャ情報の設定
			{
				if (primitive.material > -1)
				{
					const GLTFModelData::Material& material{ model_data->materials.at(primitive.material) };

					// テクスチャインデックス配列
					const int texture_indices[]
					{
						material.data.pbr_metallic_roughness.basecolor_texture.index,         // カラー
						material.data.pbr_metallic_roughness.metallic_roughness_texture.index,// 粗さと反射率
						material.data.normal_texture.index,									  // 法線
						material.data.emissive_texture.index,								  // 自身からの発光
						material.data.occlusion_texture.index,								  // 陰影
					};

					ID3D11ShaderResourceView* null_shader_resource_view{ };
					// テクスチャインデックス配列の大きさの配列を作成する
					std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

					for (int32_t texture_index = 0; texture_index < shader_resource_views.size(); ++
						texture_index)
					{
						// プリミティブに番号の種類のテクスチャが設定されている場合は情報を渡し、設定されていない場合は空の情報を渡す。
						shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
							model_data->texture_resource_views.
							at(model_data->textures.
								at(texture_indices[texture_index]).
								source).Get() :
							null_shader_resource_view;
					}

					// ピクセルシェーダーにテクスチャ情報を渡す
					dc->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()),
						shader_resource_views.data());
				}
			}

			// 描画実行
			dc->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.Count()), 0, 0);
		}
	}

	// 子ノードが存在する場合
	for (std::vector<int32_t>::value_type child_index : node->children)
	{
		DrawMeshRecursive(dc, child_index);
	}
}
