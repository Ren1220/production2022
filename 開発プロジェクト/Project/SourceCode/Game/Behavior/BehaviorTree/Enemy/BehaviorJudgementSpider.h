#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorJudgementEnemy.h"

/* --- �錾 ---- */

class JudgementButtleSpider : public BehaviorJudgement
{
public:
	JudgementButtleSpider() = default;
	~JudgementButtleSpider() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

