#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <unordered_map>
#include <optional>

/* --- 前方宣言 ---- */

class Actor;
class StateMachine;

/* --- 宣言 ---- */

class StateMachineState
{
public:
	StateMachineState(const std::shared_ptr<Actor>& actor);
	virtual ~StateMachineState();

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	virtual void Start() = 0;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	virtual void Execute(float elapsed_time) = 0;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	virtual void End() = 0;
public:

	std::shared_ptr<StateMachine> GetStateMachine() const;
	std::shared_ptr<Actor> GetActor() const;

	void StopAnimation() const;

protected:

	std::weak_ptr<Actor> w_actor; //  このステートマシンをもつアクタ

	int32_t step; // ステップ
};

class StateMachineHierarchicalState : public StateMachineState
{
public:
	StateMachineHierarchicalState(const std::shared_ptr<Actor>& actor);
	~StateMachineHierarchicalState() override;

	void Start() override = 0;
	void Execute(float elapsed_time) override = 0;
	void End() override = 0;
	/// <summary>
	/// <para>遷移前のサブステートが存在する場合はその終了処理</para>
	/// <para>サブステートを変更し							  </para>
	/// <para>遷移後のサブステートの開始処理を行う			  </para>
	/// <para>アニメーションが設定されている場合はその再生も行う</para>
	/// </summary>
	void TransitionSubState(uint32_t key);

	void RegisterSubState(uint32_t key, std::shared_ptr<StateMachineState> state);

	std::shared_ptr<StateMachineState> GetCurrentSubState();

	/// <summary>
	/// ステートマシンの更新処理で使う
	/// ステートの遷移には使わない
	/// </summary>
	void SetCurrentSubState(uint32_t key);

	const std::shared_ptr<StateMachineState>& GetSubStateOfKey(uint32_t key) const;

	/// <summary>
	/// サブステートのインデックスの取得
	/// </summary>
	/// <returns>見つからない場合nulloptを返す</returns>
	std::optional<uint32_t> FindCurrentSubStateKey();

protected:

	/// <summary>
	/// サブステートの実行処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void ExecuteSubState(float elapsed_time) const;

protected:

	std::unordered_map<uint32_t, std::shared_ptr<StateMachineState>> sub_states; // 階層ステートの持つサブステートのバッファ
	std::shared_ptr<StateMachineState> current_sub_state;               // 現在実行されているサブステート
};
