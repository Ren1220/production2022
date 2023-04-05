#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorAction.h"

/* --- 宣言 ---- */

class ActionPursuitSkeleton : public BehaviorAction
{
public:

	ActionPursuitSkeleton() = default;
	~ActionPursuitSkeleton() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionAttackSkeleton : public BehaviorAction
{
public:

	ActionAttackSkeleton() = default;
	~ActionAttackSkeleton() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;

};

class ActionWanderSkeleton : public BehaviorAction
{
public:

	ActionWanderSkeleton() = default;
	~ActionWanderSkeleton() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
private:

	void SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const;

};

class ActionDamagedSkeleton : public BehaviorAction
{
public:

	ActionDamagedSkeleton() = default;
	~ActionDamagedSkeleton() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionDeadSkeleton : public BehaviorAction
{
public:

	ActionDeadSkeleton() = default;
	~ActionDeadSkeleton() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionFaceSkeleton : public BehaviorAction
{
public:

	ActionFaceSkeleton() = default;
	~ActionFaceSkeleton() override = default;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};
