#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class PlayerController
{
public:

	PlayerController();
	~PlayerController();

	void Initialize();
	void Update(float elapsed_time);
	void EndProcess();

	/// <summary>
	/// プレイヤーが死亡したときに呼ばれる処理
	/// </summary>
	void DeadPlayer();

	void ImGuiRender();

	void SetPlayer(const std::shared_ptr<Actor>& player);

	/// <summary>
	/// ActorManagerで名前検索も可能だが、参照する回数が多くなりそうなのでこちらの方がたぶんよい
	/// </summary>
	std::shared_ptr<Actor> GetPlayer() const;

	/// <summary>
	/// ターゲット機能を用いて現在ターゲットしている敵
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Actor> GetTarget() const;

	/// <summary>
	/// フィールド上のアイテムを拾える範囲
	/// </summary>
	float GetItemPickupRange() const { return item_pickup_range; }

	std::shared_ptr<Actor> GetJumpAttackTarget() const;
	void SelectJumpAttackTarget(const std::vector<std::weak_ptr<Actor>>& w_attackers);
	void ClearJumpAttackTarget();

	bool CanTransitionJumpAttack() const { return can_transition_jump_attack; }
	void SetTransitionJumpAttack(bool b) { can_transition_jump_attack = b; }

private:

	/// <summary>
	/// (未実装)敵のターゲット
	/// </summary>
	void TargetEnemy();

	/// <summary>
	/// プレイヤーからの距離、その対象に対しての向き具合によって評価値を決め、
	/// 最も高い対象を返す。
	/// </summary>
	std::shared_ptr<Actor> SearchTarget();

	std::weak_ptr<Actor> w_player;
	std::weak_ptr<Actor> w_target;
	std::weak_ptr<Actor> w_jump_attack_target;
	float item_pickup_range;

	bool can_transition_jump_attack;
};
