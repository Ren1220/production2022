#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "BehaviorEnemy.h"

/* --- 宣言 ---- */

class BehaviorBoss : public BehaviorEnemy
{
public:

	// ボスの状態 体力が1/3 切るごとに変化する
	enum Mode { Usual, Enraged, Desperate };

	BehaviorBoss();
	~BehaviorBoss() override;

	void Start() override;
	void Update(float elapsed_time) override;

	void ImGuiRender() override;

	Mode GetMode() const { return mode; }
	float GetTransitionBackRange() const { return transition_back_range; }

	bool IsModeChanged() const { return is_mode_changed; }
	void SetModeChanged(bool b) { is_mode_changed = b; }

	bool IsFindPlayer() const { return is_find_player; }
	void SetFindPlayer(bool b) { is_find_player = b; }
	
	float GetSearchRange() const { return search_range; }
	float GetAttackRange() const { return attack_range; }
	void SetAttackRange(float f) { attack_range = f; }

	float CalcDistanceToInitPos() const;
	const Vector3& GetInitPos() const { return init_pos; }

private:

	Mode mode;
	bool is_mode_changed; // 状態が変化したタイミング

	float transition_back_range;

	float search_range;
	bool is_find_player;

	float attack_range;

	Vector3 init_pos; // 初期位置

};

