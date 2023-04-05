#include "ProjectileRock.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../Actor/Actor.h"
#include "../../Physics/RigidBody.h"
#include "../../General/BattleSystem.h"

/* --- 定義 ---- */

ProjectileRock::ProjectileRock(const std::shared_ptr<Actor>& owner, int32_t power, const Vector3& direction, float speed, float max_speed, float alive_time)
	: Projectile(owner, alive_time), power(power), direction(direction), speed(speed), max_speed(max_speed)
{
}

void ProjectileRock::Start()
{
	auto actor{ GetActor() };

	Quaternion rot{ actor->GetWorldRotation() };
	Vector3 pos{ actor->GetWorldPosition() };

	// 投射方向から角度を算出
	Vector3 a{ 0.0f, 0.0f, -1.0f };
	Vector3 b{ direction };
	float angle{ a.Dot(b) };

	Vector3 cross(a.Cross(b));
	if (cross.y < 0)
	{
		angle += 1.57f;
	}

	rock_efk.Initialize("RockBullet/rock_bullet.efk", pos);
	rock_efk.SetRotation(Vector3(0, 1, 0), angle);
}

void ProjectileRock::Update(float elapsed_time)
{
	RemoveWhenCollideWall();

	if (auto rigid_body = GetActor()->GetRigidBody())
	{
		rigid_body->AddMoveDirectionForce(direction, speed, max_speed);
	}
	rock_efk.SetPosition(GetActor()->GetWorldPosition());

	CheckAlived(elapsed_time);
}

void ProjectileRock::ImGuiRender()
{
}

void ProjectileRock::OnHitEvent(const std::shared_ptr<Actor>& opponent)
{
	if (opponent->CompareGroupName("Player"))
	{
		DamageTo(opponent, power);
	}
}

void ProjectileRock::DamageTo(const std::shared_ptr<Actor>& dst, int32_t val)
{
	if (auto owner = w_owner.lock())
	{
		BattleSystem::GetInstance().SendCombatData(w_owner.lock(), dst, val, 1.0f, 0.5f);
	}

}
