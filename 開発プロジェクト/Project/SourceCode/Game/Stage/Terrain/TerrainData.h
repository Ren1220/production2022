#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>

/* --- 宣言 ---- */

struct TerrainData
{
	struct Rect
	{
		Rect()
			: mx(0), my(0), Mx(0), My(0) {}
		Rect(uint32_t mx, uint32_t my, uint32_t Mx, uint32_t My)
			: mx(mx), my(my), Mx(Mx), My(My) {}

		uint32_t Width() const { return Mx - mx; }
		uint32_t Height() const { return My - my; }

		uint32_t mx, my, Mx, My;
	};

	struct Area : public Rect
	{
		Area(uint32_t mx, uint32_t my, uint32_t Mx, uint32_t My)
			: Rect(mx, my, Mx, My), room(), can_split_h(true), can_split_v(true) {}

		std::shared_ptr<Area> CreateChild()
		{
			return std::make_shared<Area>(mx, my, Mx, My);
		}

		bool can_split_h, can_split_v; // 区域を各方向に分割可能か

		Rect room; // 区域内に配置する部屋の大きさ
	};

	struct Walkway
	{
		Walkway(bool is_v,
			const std::shared_ptr<Area>& area0,
			const std::shared_ptr<Area>& area1)
			: is_v(is_v)
		{
			area[0] = area0; area[1] = area1;
		}

		bool is_v; // 縦か横か
		std::shared_ptr<Area> area[2]; // 間に道を生成する区域
		Rect way[3];  // way0 --- way1 --- way2
	};

	std::vector<std::shared_ptr<Area>> areas;
	std::vector<std::shared_ptr<Walkway>> walkways;
};
