#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorAction.h"

/* --- 宣言 ---- */

class ActionDeadBoss : public BehaviorAction
{
public:

	ActionDeadBoss() = default;
	~ActionDeadBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionDamagedBoss : public BehaviorAction
{
public:

	ActionDamagedBoss() = default;
	~ActionDamagedBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionIdleBoss : public BehaviorAction
{
public:

	ActionIdleBoss() = default;
	~ActionIdleBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionRoaringBoss : public BehaviorAction
{
public:

	ActionRoaringBoss() = default;
	~ActionRoaringBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionRoaringBattleBoss : public BehaviorAction
{
public:

	ActionRoaringBattleBoss() = default;
	~ActionRoaringBattleBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionPursuitBoss : public BehaviorAction
{
public:

	ActionPursuitBoss() = default;
	~ActionPursuitBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionBackBoss : public BehaviorAction
{
public:

	ActionBackBoss() = default;
	~ActionBackBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionFaceBoss : public BehaviorAction
{
public:

	ActionFaceBoss() = default;
	~ActionFaceBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionAttack0Boss : public BehaviorAction
{
public:

	ActionAttack0Boss() = default;
	~ActionAttack0Boss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionAttack1Boss : public BehaviorAction
{
public:

	ActionAttack1Boss() = default;
	~ActionAttack1Boss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionTackleBoss : public BehaviorAction
{
public:

	ActionTackleBoss() = default;
	~ActionTackleBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionLaunchBoss : public BehaviorAction
{
public:

	ActionLaunchBoss() = default;
	~ActionLaunchBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionJumpATKBoss : public BehaviorAction
{
public:

	ActionJumpATKBoss() = default;
	~ActionJumpATKBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionReturnBoss : public BehaviorAction
{
public:

	ActionReturnBoss() = default;
	~ActionReturnBoss() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;

};