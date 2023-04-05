#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Actor.h"

/* --- 宣言 ---- */

class ActorPlayer : public Actor
{
public:

	enum State : uint32_t
	{
		MovementState,
		_IdleState,
		_RunState,
		_PickupStandState,
		_PickupRunState,
		AttackState,
		_MeleeAttackState,
		_ParryState,
		_RollState,
		_AttackCombo0,
		_AttackCombo1,
		_AttackCombo2,
		_AttackSlash,
		_JumpAttackState,
		DamagedState,
		DeathState,
		ConsumeState,
		EnchantState,
	};

	enum AnimationKey : uint32_t
	{
		Idle,
		Run,
		Attack,
		Damaged,
		Dead0,
		Dead1,
		PickupStand,
		PickupRun,
		Parry,
		Roll,
		AttackCombo0,
		AttackCombo1,
		AttackCombo2,
		AttackSkill0,
		JumpAttack,
		Consume,
		Enchant,
		Walk,
	};

	ActorPlayer()
		: Actor() {}

	void Initialize(
		ID3D11Device* device,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(), 
		const Vector3& scale = { 1, 1, 1 });

private:

	void RegisterAnimation();

};
