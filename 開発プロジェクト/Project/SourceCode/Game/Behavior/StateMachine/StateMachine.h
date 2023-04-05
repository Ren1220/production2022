#pragma once

/* --- 概要 ---- */



/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <unordered_map>
#include <optional>

#include "Game/Behavior/Behavior.h"

/* --- 前方宣言 ---- */

class StateMachineState;
class StateMachineHierarchicalState;

/* --- 宣言 ---- */

class StateMachine : public Behavior
{
public:

	StateMachine(uint32_t state_key);
	StateMachine(uint32_t state_key, uint32_t sub_state_key);

	virtual ~StateMachine() override;

	void Start()override;
	void Update(float elapsed_time) override;
	void End() override;
	void ImGuiRender() override;

private:

	void UpdateState();
	void TransitionStateAndSubState(uint32_t state_key, uint32_t sub_state_key);

public:

	/// <summary>
	/// 遷移前のステートが存在する場合はその終了処理
	/// ステートを変更し							 
	/// 遷移後のステートの開始処理を行う			 
	/// アニメーションが設定されている場合はその再生も行う
	/// </summary>
	void TransitionState(uint32_t key);

	/// <summary>
	/// <para>遷移前サブステートが存在する場合はその終了処理</para>
	/// <para>サブステートを変更し							  </para>
	/// <para>遷移後のサブステートの開始処理を行う			  </para>
	/// <para>アニメーションが設定されている場合はその再生も行う</para>
	/// </summary>
	void TransitionSubState(uint32_t key);

	/// <summary>
	/// <para>ヒエラルキーステートを登録する</para>
	/// <para>同時に、そのステートに遷移するときに再生するアニメーションを設定する</para>
	/// <para>なお、処理の順番上サブステートのアニメーション設定が優先される</para>
	/// </summary>
	void RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state, uint32_t animation_key, bool animation_loop_flag);
	void RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state);

	/// <summary>
	/// サブステートを登録する
	/// 同時に、そのステートに遷移するときに再生するアニメーションを設定する
	/// なお、処理の順番上サブステートのアニメーション設定が優先される
	/// </summary>
	void RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state, uint32_t animation_key, bool animation_loop_flag);
	void RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state);

	/// <summary>
	/// 次の更新時に引数のステートに遷移する
	/// </summary>
	void SetNextState(uint32_t key);

	/// <summary>
	/// 次の更新時に引数のサブステートに遷移する
	/// </summary>
	void SetNextSubState(uint32_t key);

	uint32_t FindCurrentStateKey() const;
	std::shared_ptr<StateMachineState> GetCurrentState();
	const std::shared_ptr<StateMachineHierarchicalState>& GetStateOfKey(uint32_t key) const;

	uint32_t GetCurrentStateKey() const { return current_state_key; };
	const std::optional<uint32_t>& GetCurrentSubStateKey() const { return current_sub_state_key; };

	/// <summary>
	/// ステート遷移可能か
	/// </summary>
	bool CanTransition() const { return can_transition; }
	void SetTransitionFlag(bool b) { can_transition = b; }

protected:

	uint32_t current_state_key;						// 現在のステートのキー
	uint32_t next_state_key;						// 次の  ステートのキー
	std::optional<uint32_t> current_sub_state_key;	// 現在のサブステートのキー
	std::optional<uint32_t> next_sub_state_key;		// 次の  サブステートのキー

	std::shared_ptr<StateMachineHierarchicalState> current_state;       // 現在のステート
	std::unordered_map<uint32_t, std::shared_ptr<StateMachineHierarchicalState>> registered_states; // 登録したステート

	bool can_transition; // ステート遷移可能

};
