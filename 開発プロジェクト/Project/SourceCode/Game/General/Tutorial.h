#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>

#include "Utility/Singleton.h"
#include "Utility/Trits.h"

/* --- 宣言 ---- */

class Tutorial : public Singleton<Tutorial>
{
public:

	enum Flag : uint8_t
	{
		ShowAttackText , // 通常攻撃のチュートリアル
		ShowComboText ,  // コンボのチュートリアル
		ShowSlashText  , // 強攻撃
		ShowDodgeText  , // 回避行動
		ShowCounterText, // 反撃（ジャンプ攻撃）
		ShowEnchantText, // エンチャント
		ShowEnchantText2, // エンチャント(追加のテキスト)
		ShowConsumeText, // 消費
		ElementCount,
	};

	enum State : uint8_t
	{
		NotDisplayed,		// 未表示
		Displaying,			// 表示中
		AlreadyDisplayed,	// 表示済み
	};

	Tutorial()
		: flags(), is_tutorial(false) {}
	~Tutorial() = default;

	void ResetFlag()
	{
		flags = {};
	}

	/// <summary>
	/// フラグの設定
	/// </summary>
	/// <param name="f"></param>
	/// <param name="t">0 未表示, 1 表示中, 2 表示済み</param>
	void SetFlag(Flag f, State t)
	{
		flags.Set(f, t);
	}

	/// <summary>
	/// フラグの取得
	/// </summary>
	/// <returns>0 未表示, 1 表示中, 2 表示済み</returns>
	State GetFlag(Flag f) const
	{
		return static_cast<State>(flags[f]);
	}

	bool IsTutorial() const { return is_tutorial; }
	void SetTutorial(bool b) { is_tutorial = b; }

private:

	bool is_tutorial;

	Trits<ElementCount> flags;
};
