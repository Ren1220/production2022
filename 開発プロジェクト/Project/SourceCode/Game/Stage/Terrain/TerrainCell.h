#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>

/* --- 宣言 ---- */

class TerrainCell
{
public:

	enum State : uint8_t
	{
		None     = 0,	   // セルなし
		Traces   = 1 << 0, // いずれかの世代で生存セルになっていた。
		Alive    = 1 << 1, // 生存状態
		Dead	 = 1 << 2, // 死亡状態
		Obstacle = 1 << 3, // 周りのセルの干渉を受けない状態
	};

	TerrainCell()
		: state(Dead) { }

	State& operator=(uint8_t rhs) { return state = static_cast<State>(rhs); }
	State& operator|=(uint8_t rhs) { return state = static_cast<State>(state | rhs); }
	State& operator&=(uint8_t rhs) { return state = static_cast<State>(state & rhs); }
	State operator|(uint8_t rhs) const { return static_cast<State>(state | rhs); }
	State operator&(uint8_t rhs) const { return static_cast<State>(state & rhs); }
	State operator~() const { return static_cast<State>(~state); }

	/// <summary>
	/// マスクの部分のビットのみ書き換える
	/// </summary>
	void Set(State src, uint8_t mask)
	{
		state = static_cast<State>((state & ~mask) | src);
	}

	/// <summary>
	/// マスクの部分のビットのみで比較を行う
	/// </summary>
	bool Compare(State src, uint8_t mask) const
	{
		return ((state & mask) == src);
	}

	State Get() const { return state; }

private:

	State state;
};
