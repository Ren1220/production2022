#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "GLTFModel.h"

#include "../Resource/ConstantBuffer.h"

/* --- 宣言 ---- */

class GLTFStaticMesh : public GLTFModel
{
public:

	GLTFStaticMesh(ID3D11Device* device, const char* filepath);
	virtual ~GLTFStaticMesh();

	/// <param name="shader">シェーダを設定しない場合は汎用シェーダーを利用する</param>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const override;
	/// <summary>
	/// インスタンス描画では、モデルデータのマテリアル構造体バッファは機能しない
	/// （メッシュによってテクスチャが存在しないものがある）ため、各メッシュの情報からビューを
	/// 作成する必要がある。尚、現状複数のメッシュを持つモデルのインスタンス描画は未実装。
	/// TODO : インスタンス描画用モデルを作る。
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="material_structured_buffer_view"></param>
	void Render(ID3D11DeviceContext* dc, Shader* shader, ID3D11ShaderResourceView* material_structured_buffer_view) const ;

private:

	/// <summary>
	/// ノードインデックスのメッシュを描画する再帰関数
	/// </summary>
	void DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const;

	struct CBufferPrimitive
	{
		static constexpr uint32_t slot{ 0 };

		Matrix world;
		int material;
		int has_tangent;
		int skin{ -1 };
		int pad;
	};

	ConstantBuffer<CBufferPrimitive> primitive_cb;
};
