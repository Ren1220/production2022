#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <vector>

#include "Terrain/Terrain.h"

/* --- 宣言 ---- */

class StageData
{
public:
	struct Header
	{
		uint32_t width;
		uint32_t height;
	};

	struct Data
	{
		std::vector<TileData> data;
	};


	void Export(const char* export_name, const Terrain& terrain);
	void Import(const char* import_name);

	const Header& GetHeader() const { return header; }
	const Data& GetData() const { return data; }

private:

	void FetchData(const Terrain& terrain);

	Header header;
	Data data;

};
