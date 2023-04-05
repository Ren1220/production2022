#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <functional>
#include <memory>
#include <vector>

#include "Utility/Singleton.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class BattleSystem: public Singleton<BattleSystem>
{
public:

	void Initialize();

	/// <summary>
	/// 戦闘処理の前に各アクタの被ダメージフラグをリセットする
	/// </summary>
	void ResetHealthFlags();

	/// <summary>
	/// 送られてきた戦闘処理を実行する
	/// </summary>
	void CombatProcess();

	/// <summary>
	/// ダメージイベントを送り、後でまとめてダメージ処理を行う
	/// </summary>
	/// <param name="attacker">攻撃側</param>
	/// <param name="target">攻撃対象</param>
	/// <param name="damage">与えるダメージ</param>
	/// <param name="invinsible_time">設定する無敵時間</param>
	/// <param name="camera_shake_time">カメラシェイク時間</param>
	/// <param name="combat_callback">追加で実行する関数 void（Actor* attacker, Actor* target, bool is_damaged）</param>
	void SendCombatData(
		const std::shared_ptr<Actor>& attacker,
		const std::shared_ptr<Actor>& target,
		int32_t damage,
		float invinsible_time = 0.1f,
		float camera_shake_time = 0.2f,	
		const std::function<void(Actor*, Actor*, bool)>& combat_callback = nullptr);

private:

	struct CombatData
	{
		std::weak_ptr<Actor> w_attacker; // 攻撃側
		std::weak_ptr<Actor> w_target;   // 攻撃対象

		int32_t damage;				// 与えるダメージ量
		float   invinsible_time;	// 設定する無敵状態
		float   camera_shake_time;	// 設定するカメラシェイク時間

		// 追加で実行する関数 第一引数が攻撃側、第二引数が攻撃対象、第三は攻撃が成功したか
		std::function<void(Actor*, Actor*, bool)> combat_callback;
	};

	std::vector<CombatData> combat_datas;
};
