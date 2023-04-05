#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorJudgementEnemy.h"

/* --- �錾 ---- */

class JudgementBattleSkeleton : public BehaviorJudgement
{
public:
	JudgementBattleSkeleton() = default;
	~JudgementBattleSkeleton() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};