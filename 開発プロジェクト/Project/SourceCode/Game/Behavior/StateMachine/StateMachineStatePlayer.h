#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Arithmetic/Arithmetic.h"
#include "StateMachineState.h"

/* --- 宣言 ---- */

class MovementStatePlayer : public StateMachineHierarchicalState
{
public:
	MovementStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~MovementStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class RunStatePlayer : public StateMachineState
{
public:
	RunStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~RunStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;

};

class RollStatePlayer : public StateMachineState
{
public:
	RollStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~RollStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;

};

class IdleStatePlayer : public StateMachineState
{
public:
	IdleStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~IdleStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;

};

class PickupItemStandStatePlayer : public StateMachineState
{
public:
	PickupItemStandStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~PickupItemStandStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class PickupItemRunStatePlayer : public StateMachineState
{
public:
	PickupItemRunStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~PickupItemRunStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class AttackStatePlayer : public StateMachineHierarchicalState
{
public:
	AttackStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~AttackStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class ComboAttack0StatePlayer : public StateMachineState
{
public:
	ComboAttack0StatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor)
	{
	}
	~ComboAttack0StatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class ComboAttack1StatePlayer : public StateMachineState
{
public:
	ComboAttack1StatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor)
	{
	}
	~ComboAttack1StatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class ComboAttack2StatePlayer : public StateMachineState
{
public:
	ComboAttack2StatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor)
	{
	}
	~ComboAttack2StatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class SlashAttackStatePlayer : public StateMachineState
{
public:
	SlashAttackStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor)
	{
	}
	~SlashAttackStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class JumpAttackStatePlayer : public StateMachineState
{
public:
	JumpAttackStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor)
	{
	}
	~JumpAttackStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class ParryStatePlayer : public StateMachineState
{
public:
	ParryStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineState(actor) {}
	~ParryStatePlayer() override = default;

	/// <summary>
	/// ステートに入った時の関数
	/// </summary>
	void Start() override;
	/// <summary>
	/// ステートで実行する関数
	/// </summary>
	void Execute(float elapsed_time) override;
	/// <summary>
	/// ステートから出ていくときの関数
	/// </summary>
	void End() override;
};

class DamagedStatePlayer : public StateMachineHierarchicalState
{
public:

	DamagedStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~DamagedStatePlayer() override = default;

	void Start() override;
	void Execute(float elapsed_time) override;
	void End() override;
};

class DeathStatePlayer : public StateMachineHierarchicalState
{
public:
	DeathStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~DeathStatePlayer() override = default;

	void Start() override;
	void Execute(float elapsed_time) override;
	void End() override;
};

class ConsumeStatePlayer : public StateMachineHierarchicalState
{
public:
	ConsumeStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~ConsumeStatePlayer() override = default;

	void Start() override;
	void Execute(float elapsed_time) override;
	void End() override;
};

class EnchantStatePlayer : public StateMachineHierarchicalState
{
public:
	EnchantStatePlayer(const std::shared_ptr<Actor>& actor)
		: StateMachineHierarchicalState(actor) {}
	~EnchantStatePlayer() override = default;

	void Start() override;
	void Execute(float elapsed_time) override;
	void End() override;
};
