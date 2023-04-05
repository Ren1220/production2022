#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../BehaviorAction.h"

/* --- 宣言 ---- */

class ActionDamagedSpider : public BehaviorAction
{
public:

	ActionDamagedSpider() = default;
	~ActionDamagedSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionDeadSpider : public BehaviorAction
{
public:

	ActionDeadSpider() = default;
	~ActionDeadSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};


class ActionPursuitSpider : public BehaviorAction
{
public:

	ActionPursuitSpider() = default;
	~ActionPursuitSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};


class ActionFaceSpider : public BehaviorAction
{
public:

	ActionFaceSpider() = default;
	~ActionFaceSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionApproachSpider : public BehaviorAction
{
public:

	ActionApproachSpider() = default;
	~ActionApproachSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionAttackSpider : public BehaviorAction
{
public:

	ActionAttackSpider() = default;
	~ActionAttackSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
};

class ActionLaunchSpider: public BehaviorAction
{
public:

	ActionLaunchSpider() = default;
	~ActionLaunchSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;

};

class ActionWanderSpider : public BehaviorAction
{
public:

	ActionWanderSpider() = default;
	~ActionWanderSpider() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;

	State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) override;
private:

	void SetWanderGoalPoint(const std::shared_ptr<Actor>& actor) const;

};
