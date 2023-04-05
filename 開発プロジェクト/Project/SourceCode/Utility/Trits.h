#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>

/* --- 宣言 ---- */

/// <summary>
/// 三進数をコンパクトに扱うクラス生成時に配列数を指定する必要がある
/// </summary>
/// <typeparam name="s"></typeparam>
template<size_t s>
class Trits
{
public:

	Trits() = default;

	template<class...Args>
	Trits(Args... args)
		: blocks()
	{
		static_assert(sizeof...(args) == s, "The size of the array and the number of lements in the argument must be the same.");

		for (size_t ei = 0, bi = 0; uint8_t t : { args... })
		{
			blocks[bi] += t * static_cast<int>(pow(3, ei));
			// 一ブロックに５つのデータが格納される
			if (ei++ >= 4) { ei = 0; ++bi; }
		}
	}

	uint8_t operator[](size_t i) const
	{
		_ASSERT_EXPR(i < s, L"配列外へのアクセス");
		return (blocks[i / 5ULL] / static_cast<uint8_t>(pow(3, i % 5ULL))) % 3ULL;
	}

	uint8_t Get(size_t i)
	{
		_ASSERT_EXPR(i < s, L"配列外へのアクセス");
		return (blocks[i / 5ULL] / static_cast<uint8_t>(pow(3, i % 5ULL))) % 3ULL;
	}

	void Set(size_t i, uint8_t t)
	{
		_ASSERT_EXPR(i < s, L"配列外へのアクセス");
		_ASSERT_EXPR(t < 3, L"三進数なので0~2の値");

		size_t b{ i / 5ULL };
		int p{ static_cast<int>(pow(3, i % 5ULL)) };
		blocks[b] -= Get(i) * p; // フラグ部分の値をゼロに
		blocks[b] += t * p;   // フラグを立てる
	}

private:

	uint8_t blocks[(s / 5ULL) + 1ULL];
};
