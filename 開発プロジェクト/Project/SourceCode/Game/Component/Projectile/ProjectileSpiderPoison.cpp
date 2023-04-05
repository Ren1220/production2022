#include "ProjectileSpiderPoison.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

#include "../../Actor/Actor.h"
#include "../../Actor/ActorManager.h"
#include "../../Physics/RigidBody.h"
#include "../../General/GameSystem.h"
#include "../../General/BattleSystem.h"
#include "../Health/Health.h"

/* */

ProjectileSpiderPoison::ProjectileSpiderPoison(
	const std::shared_ptr<Actor>& owner, 
	int32_t power,
	const Vector3& direction,
	float speed,
	float max_speed,
	float alive_time)
	: Projectile(owner, alive_time), power(power), direction(direction), speed(speed), max_speed(max_speed)
{

}

void ProjectileSpiderPoison::Start()
{
	auto actor{ GetActor() };

	Quaternion rot{ actor->GetWorldRotation() };
	Vector3 pos{ actor->GetWorldPosition() };
	pos.y -= 0.5f;
	// クォータニオンを回転軸と回転量に分解
	//float c{ acosf(rot.w) };

	//float angle{ 2.0f * c * (180.0f / static_cast<float>(std::numbers::pi)) };
	//Vector3 axis;

	//axis.x = rot.x / sinf(c);
	//axis.y = rot.y / sinf(c);
	//axis.z = rot.z / sinf(c);

	// 投射方向から角度を算出
	Vector3 a{ 0.0f, 0.0f, -1.0f };
	Vector3 b{ direction };
	float angle{ a.Dot(b) };

	Vector3 cross(a.Cross(b));
	if (cross.y < 0)
	{
		angle += 1.57f;
	}

	EffekseerHandle handle("Poison/poison_launch.efk", pos);
	handle.SetRotation(Vector3(0, 1, 0), angle);
	handle.SetScale({ 0.1f, 0.1f, 0.1f });
}

void ProjectileSpiderPoison::Update(float elapsed_time)
{
	RemoveWhenCollideWall();

	if (auto rigid_body = GetActor()->GetRigidBody())
	{
		rigid_body->AddMoveDirectionForce(direction, speed, max_speed);
	}

	CheckAlived(elapsed_time);
}

void ProjectileSpiderPoison::ImGuiRender()
{
}

void ProjectileSpiderPoison::OnHitEvent(const std::shared_ptr<Actor>& opponent)
{
	if (opponent->CompareGroupName("Player"))
	{
		DamageTo(opponent, power);

		Vector3 pos{ opponent->GetWorldPosition() };
		pos.y += 1.0f;

		EffekseerHandle handle("Poison/poison_hit.efk", pos); 

		auto actor{ GetActor() };
		//Quaternion rot{ actor->GetWorldRotation() };

		//float c{ acosf(rot.w) };

		//float angle{ 2.0f * c /** (180.0f / static_cast<float>(std::numbers::pi))*/ };
		//Vector3 axis;

		//axis.x = rot.x / sinf(c);
		//axis.y = rot.y / sinf(c);
		//axis.z = rot.z / sinf(c);
		//
	// 投射方向から角度を算出
		Vector3 a{ 0.0f, 0.0f, -1.0f };
		Vector3 b{ direction };

		float angle{ a.Dot(b) };

		Vector3 cross(a.Cross(b));
		if (cross.y < 0)
		{
			angle += 1.57f;
		}

		handle.SetRotation(Vector3(0, 1, 0), angle);

		GameSys.GetActorManager()->Remove(actor);
	}
}

void ProjectileSpiderPoison::DamageTo(const std::shared_ptr<Actor>& dst, int32_t val)
{
	if (auto owner = w_owner.lock())
	{
		BattleSystem::GetInstance().SendCombatData(w_owner.lock(), dst, val, 1.0f, 0.5f,
			[](Actor* atk, Actor* tgt, bool is_damaged)
			{
				if (is_damaged)
				{
					// 毒状態にする
					tgt->GetComponent<Health>()->SetEffect(HealthEffectID::Poisonous, 5.0f);
				}
			});
	}
}
