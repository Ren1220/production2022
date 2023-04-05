#include "Projectile.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../../Actor/Actor.h"
#include "../../Actor/ChildActorParam.h"
#include "../../Actor/ActorManager.h"
#include "../../Physics/RigidBody.h"
#include "../../General/GameSystem.h"
#include "../../General/BattleSystem.h"
#include "../Health/Health.h"


/* --- ��` ---- */

Projectile::Projectile(const std::shared_ptr<Actor>& owner, float alive_time)
	: Component(), w_owner(owner), alive_time(alive_time), timer()
{
}

void Projectile::CheckAlived(float elapsed_time)
{
	// �������Ԃ𒴂�����폜
	if (timer > alive_time)
	{
		GameSys.GetActorManager()->Remove(GetActor());
	}

	timer += elapsed_time;
}

void Projectile::RemoveWhenCollideWall()
{
	auto actor{ GetActor() };

	if (actor->GetRigidBody()->IsCollideWall())
	{
		GameSys.GetActorManager()->Remove(actor);
	}
}

void Projectile::DamageTo(const std::shared_ptr<Actor>& dst, int32_t val)
{
	if (auto owner = w_owner.lock())
	{
		BattleSystem::GetInstance().SendCombatData(owner, dst, val, 1.0f, 0.5f);
	}
}
