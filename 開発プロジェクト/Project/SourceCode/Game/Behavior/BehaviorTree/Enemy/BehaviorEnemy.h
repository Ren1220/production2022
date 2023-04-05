#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorTree.h"

/* --- 宣言 ---- */

class BehaviorEnemy: public BehaviorTree
{
public:

	BehaviorEnemy();
	~BehaviorEnemy() override;

	virtual void Start() override;
	virtual void Update(float elapsed_time) override;

	virtual void ImGuiRender() override;

public:

	/// <summary>
	/// 前のフレームでダメージを受けたことで中断されたときにダメージステートに遷移する際に使う。
	/// ビヘイビア内で被ダメにより中断->Health更新->Inferenceでのステートを決定という風に処理されるため。
	/// </summary>
	void SetDamaged(bool b = true) { is_damaged = b; }

	/// <summary>
	/// 前のフレームでダメージを受けたことで中断されたときにダメージステートに遷移する際に使う。
	/// ビヘイビア内で被ダメにより中断->Health更新->Inferenceでのステートを決定という風に処理されるため。
	/// </summary>
	bool IsDamaged() const { return is_damaged; }

	/// <summary>
	/// プレイヤーへのベクトル
	/// </summary>
	const Vector3& GetPlayerVector() const { return player_vector; }

protected:

	// 前のフレームでダメージを受けたことで中断されたときにダメージステートに遷移する際に使う。
	// ビヘイビア内で被ダメにより中断->Health更新->Inferenceでのステートを決定という風に処理されるため。
	bool is_damaged; 

	// プレイヤーとのベクトルはよく使うので計算しておく
	Vector3 player_vector;
};

