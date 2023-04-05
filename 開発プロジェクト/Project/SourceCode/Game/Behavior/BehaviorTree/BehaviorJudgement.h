#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <string>

#include "../../Actor/Actor.h"

/* --- �錾 ---- */

class BehaviorJudgement
{
public:

	BehaviorJudgement() = default;
	virtual ~BehaviorJudgement() = default;

	virtual void Initialize(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// ���̃m�[�h�ɑJ�ڂ��\�ȏ�Ԃ����肷��B
	/// </summary>
	/// <returns></returns>
	virtual bool Judgement(const std::shared_ptr<Actor>& actor) = 0;

};
