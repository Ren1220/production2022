#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorAction.h"

/* --- 宣言 ---- */

class ActionAppearanceWolf : public BehaviorAction
{
public:

	ActionAppearanceWolf() = default;
	~ActionAppearanceWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionPursuitWolf: public BehaviorAction
{
public:

	ActionPursuitWolf() = default;
	~ActionPursuitWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionAttackWolf : public BehaviorAction
{
public:

	ActionAttackWolf() = default;
	~ActionAttackWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;
	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;

};

class ActionSummonWolf : public BehaviorAction
{
public:

	ActionSummonWolf() = default;
	~ActionSummonWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;
	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;

private:

	void SummonWolf();
};

class ActionWanderWolf : public BehaviorAction
{
public:

	ActionWanderWolf() = default;
	~ActionWanderWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
private:

	void SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const;

};

class ActionDamagedWolf : public BehaviorAction
{
public:

	ActionDamagedWolf() = default;
	~ActionDamagedWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionDeadWolf : public BehaviorAction
{
public:

	ActionDeadWolf() = default;
	~ActionDeadWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionFaceWolf : public BehaviorAction
{
public:

	ActionFaceWolf() = default;
	~ActionFaceWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionFaceSummonWolf : public BehaviorAction
{
public:

	ActionFaceSummonWolf() = default;
	~ActionFaceSummonWolf() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};
