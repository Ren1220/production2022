#include "BehaviorJudgementSkeleton.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "BehaviorSkeleton.h"

/* --- ��` ---- */

bool JudgementBattleSkeleton::Judgement(const std::shared_ptr<Actor>& actor)
{
	auto behavior{ actor->GetBehavior<BehaviorSkeleton>() };

	return behavior->GetPlayerVector().LengthSquare() < behavior->GetSearchRange() * behavior->GetSearchRange();
}
