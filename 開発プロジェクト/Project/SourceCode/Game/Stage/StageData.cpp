#include "StageData.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <fstream>

/* --- 定義 ---- */

void StageData::Export(const char* export_name, const Terrain& terrain)
{
	auto file{ std::fstream(export_name, std::ios::out | std::ios::binary) };

	// ヘッダ情報の設定
	header.width = terrain.GetMapWidth();
	header.height = terrain.GetMapHeight();
	// マップ情報の設定
	FetchData(terrain);

	file.write(reinterpret_cast<char*>(&header), sizeof(Header));
	file.write(reinterpret_cast<char*>(data.data.data()), sizeof(TileData) * data.data.size());
	file.close();
}

void StageData::Import(const char* import_name)
{
	auto file{ std::ifstream(import_name, std::ios::in | std::ios::binary) };

	// ステージのヘッダ情報を読み取る
	file.read(reinterpret_cast<char*>(&header), sizeof(Header));

	// ヘッダ情報からマップサイズを計算する。
	int64_t stage_size{ header.width * header.height };
	data.data.resize(stage_size);

	// マップ情報を読み取る
	file.read(reinterpret_cast<char*>(data.data.data()), sizeof(TileData) * stage_size);

	file.close();
}

void StageData::FetchData(const Terrain& terrain)
{
	const size_t width{ static_cast<size_t>(terrain.GetMapWidth()) };
	const size_t height{ static_cast<size_t>(terrain.GetMapHeight()) };
	const auto& map{ terrain.GetMap() };

	data.data.resize(width * height);

	// マップから各タイルのデータを抽出しつつ、二次配列のデータを一次配列に変換する。
	for (size_t ri = 0U; ri < height; ++ri)
	{
		for (size_t ci = 0U; ci < width; ++ci)
		{
			data.data[ci + ri * width] = map[ci][ri].data;
		}
	}

}
