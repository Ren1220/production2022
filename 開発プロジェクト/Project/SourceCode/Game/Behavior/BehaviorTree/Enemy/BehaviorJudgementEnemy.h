#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../BehaviorJudgement.h"

/* --- �錾 ---- */

/// <summary>
/// �_���[�W�����Behavior�N���X���̃t���O��p����B
/// �_���[�W���󂯂����Ƀm�[�h�����f����A����Inference�����s���ꂽ�ۂɂ�Health�R���|�[�l���g���X�V����Ă��邽��
/// </summary>
class JudgementDamagedEnemy : public BehaviorJudgement
{
public:
	JudgementDamagedEnemy() = default;
	~JudgementDamagedEnemy() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};

class JudgementDeadEnemy : public BehaviorJudgement
{
public:
	JudgementDeadEnemy() = default;
	~JudgementDeadEnemy() override = default;

	bool Judgement(const std::shared_ptr<Actor>& actor) override;
};
