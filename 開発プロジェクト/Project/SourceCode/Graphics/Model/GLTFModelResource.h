#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "Utility/Singleton.h"

/* --- 前方宣言 ---- */

class GLTFModelData;

/* --- 宣言 ---- */

/// <summary>
/// シングルトンGLTFモデルデータ管理クラス
/// </summary>
class GLTFModelResource : public Singleton<GLTFModelResource>
{
public:

	/// <summary>
	/// キャッシュデータを解放する
	/// </summary>
	void ReleaseCache()
	{
		model_data_cache.clear();
	};
	/// <summary>
	/// <para>モデルデータを読み込む</para>
	/// <para>キャッシュに同一のパスが存在している場合それのデータの参照を渡す</para>
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filepath"></param>
	/// <returns></returns>
	std::shared_ptr<GLTFModelData> LoadModelData(ID3D11Device* device, const char* filepath);

private:

	std::unordered_map<std::string, std::shared_ptr<GLTFModelData>> model_data_cache; // モデルデータキャッシュ
};
