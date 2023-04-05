#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <array>
#include <memory>

#include "Arithmetic/Arithmetic.h"

#include "../Shader/Shader.h"

#include "GLTFModelData.h"

/* --- 宣言 ---- */

class GLTFModel
{
public:

	GLTFModel(ID3D11Device* device, const char* filepath);
	virtual ~GLTFModel();

	/// <summary>
	/// ワールド行列を任意の座標系に変換して設定する
	/// </summary>
	void SetWorldTransform(const Matrix& world_transform, const Matrix& coordinate_space = Matrix::RhsYup())
	{
		this->world_transform = coordinate_space * world_transform;
	}

	const Matrix& GetWorldTransform() const { return world_transform; }
	std::shared_ptr<GLTFModelData> GetModelData() const { return w_model_data.lock(); }

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="shader">シェーダを設定しない場合は汎用シェーダーを利用する</param>
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const = 0;

private:

	std::weak_ptr<GLTFModelData> w_model_data; 

	Matrix world_transform; 

};

