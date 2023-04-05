#include "GLTFModel.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "GLTFModelResource.h"

/* --- 定義 ---- */

GLTFModel::GLTFModel(ID3D11Device* device, const char* filepath)
	: world_transform(Matrix::Identity())
{
	// リソース管理クラスを通して、モデルデータを読み込む
	w_model_data = GLTFModelResource::GetInstance().LoadModelData(device, filepath);
}

GLTFModel::~GLTFModel()
{
}
