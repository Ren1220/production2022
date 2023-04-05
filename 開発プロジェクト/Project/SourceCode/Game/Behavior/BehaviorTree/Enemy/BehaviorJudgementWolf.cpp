#include "BehaviorJudgementWolf.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorWolf.h"

/* --- ��` ---- */

bool JudgementHasSummonedWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	// ���s����͈̂�x����
	if (has_called) return false;

	has_called = true;
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	// �������ꂽ�ꍇ�̂�
	if (behavior->HasSummoned()) return true;

	return false;
}

bool JudgementCanSummonWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	// ���s����͈̂�x����
	if (has_called) return false;

	has_called = true;
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };
	// ��������Ă��Ȃ��ꍇ�̂�
	if (!behavior->HasSummoned()) return true;

	return false;
}

bool JudgementBattleWolf::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorWolf>() };

	return behavior->GetPlayerVector().LengthSquare() < behavior->GetSearchRange() * behavior->GetSearchRange();
}
