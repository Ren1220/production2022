#include "GLTFModelResource.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Utility/ChronoTimer.h"

#include "GLTFModelData.h"

/* --- 定義 ---- */

std::shared_ptr<GLTFModelData> GLTFModelResource::LoadModelData(ID3D11Device* device, const char* filepath)
{
    auto&& c_itr { model_data_cache.find(filepath) };
    ChronoTimer timer;

    // キャッシュに同一パスのデータが存在する場合
    if (c_itr != model_data_cache.end())
    {
        // 同一パスのデータを返す
        return c_itr->second;
    }
    // キャッシュに同一パスのデータが存在するしない場合
    else
    {
        timer.Start();
        // パスのデータを読み込む
        std::shared_ptr<GLTFModelData> model_data{ std::make_shared<GLTFModelData>(device, filepath) };
        timer.Stop(true, std::format(L"[{}]の読み込み時間", std::wstring(model_data->filename.begin(), model_data->filename.end())));
        // 読み込んだデータをキャッシュに登録する
        model_data_cache.emplace(std::make_pair(filepath, model_data));
        // 読み込んだデータを返す
        return model_data;
    }
}