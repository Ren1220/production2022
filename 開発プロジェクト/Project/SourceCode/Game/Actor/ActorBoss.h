#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Actor.h"

/* --- �錾 ---- */

class ActorBoss : public Actor
{
public:

	enum AnimationKey : uint32_t
	{
		Idle,
		Dead,
		Damaged,
		RoaringBegin,
		Roaring,
		Run,
		Tackle,
		Jump,
		BackJump,
		JumpAttack,
		Walk,
		Attack0, // �p���`
		Attack1, // �U����
		Kick,    // �΂��R�Ƃ΂�
		Return,  // �����ʒu�ɃW�����v
	};


	ActorBoss()
		: Actor()
	{

	}

	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });

private:

	void RegisterAnimation();
};
