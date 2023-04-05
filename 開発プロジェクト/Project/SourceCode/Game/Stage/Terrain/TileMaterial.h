#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>

/* --- 宣言 ---- */

class TileMaterial
{
public:

	using Index = uint8_t;

	enum class Floor : Index
	{
		Default = 0,
		Warp,
		ElementCount, // 要素数
	};

	enum class Wall : Index
	{
		Default = 0,
		ElementCount, // 要素数
	};

	TileMaterial()
		: index() {}

	TileMaterial(Floor rhs)
		: index(static_cast<Index>(rhs)) {}
	TileMaterial(Wall rhs)
		: index(static_cast<Index>(rhs)) {}

	Index& operator=(Floor rhs) { return index = static_cast<Index>(rhs); }
	Index& operator=(Wall rhs) {  return index = static_cast<Index>(rhs); }

	bool operator==(Floor rhs) { return index == static_cast<Index>(rhs); }
	bool operator==(Wall rhs) {  return index == static_cast<Index>(rhs); }


	Index index;
};

