#include "RigidBody.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../Actor/Actor.h"
#include "../Stage/StageManager.h"
//#include "Collision.h"
//#include "GameSystem/GameSystem.h"

/* --- 定義 ---- */

RigidBody::RigidBody(float mass, bool is_effect_gravity) :
	is_effect_gravity(is_effect_gravity), on_ground(false), is_collide_wall(false), mass(mass), velocity(0.0f, 0.0f, 0.0f),
	acceleration(0.0f, 0.0f, 0.0f), resultant(0.0f, 0.0f, 0.0f), dynamic_friction(10.0f)
{
}

RigidBody::~RigidBody() = default;

void RigidBody::Start()
{
}

void RigidBody::Update(float elapsed_time)
{
	//if (std::shared_ptr<Actor> stage{ActorManager::GetInstance().FindActor("Stage")};
	//	Collision::RayCastOnModel(position, end, stage->GetModel(), &intersect, &intersect_normal))
	//{
	//	GetActor()->SetPosition(intersect);
	//	velocity.y     = 0;
	//	acceleration.y = 0;

	//	on_ground = true;
	//}
	//else
	//{
	//	on_ground = false;
	//}

	// 重力処理
	if (is_effect_gravity && !on_ground)
	{
		AddForce(Vector3(0.0f, gravitational_acceleration, 0.0f) * mass );
	}

	if (auto actor = GetActor())
	{
		if (Vector3 position = actor->GetWorldPosition(); position.y <= 0.0f)
		{
			position.y = 0.0f;
			//velocity.y = 0.0f;

			actor->SetWorldPosition(position);

			on_ground = true;
		}
		else
		{
			on_ground = false;
		}
	}

	//is_hit_wall = GameSystem::GetInstance().GetStageManager().IntersectModelVSTerrain(GetActor().get(), 1.0f);


	if (on_ground)
	{
		// 動摩擦処理
		UpdateFriction(dynamic_friction);
	}

	// このフレームにおける合力による移動処理
	Integrate(elapsed_time);
}

void RigidBody::ImGuiRender()
{
	ImGui::Text("Mass : %f", mass);
	ImGui::InputFloat3("Velocity", &velocity.x, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Acceleration", &acceleration.x, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Resultant", &resultant.x, 0, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::RadioButton("EffectOfGravity", is_effect_gravity))
	{
		is_effect_gravity = !is_effect_gravity;
	}
}

void RigidBody::Collide(RigidBody* opponent, float restitution, float penetration)
{
	// 自身の変化前の位置
	const Vector3 pre_position_a{ GetActor()->GetWorldPosition() };
	// 相手の変化前の位置
	const Vector3 pre_position_b{ opponent->GetActor()->GetWorldPosition() };

	// 自身と相手の衝突方向ベクトル
	const Vector3 direction_vector_ab{ (pre_position_b - pre_position_a).Normalize() };

	// 自身の衝突方向の速度成分
	const Vector3 collision_vector_velocity_ab{ velocity.Dot(direction_vector_ab) * direction_vector_ab };
	// 相手の衝突方向の速度成分
	const Vector3 collision_vector_velocity_ba{ opponent->velocity.Dot(-direction_vector_ab) * (-direction_vector_ab) };

	// 自身の衝突後の速度
	const Vector3 velocity_a{ (collision_vector_velocity_ba - collision_vector_velocity_ab) * (1.0f + restitution) / (mass / opponent->mass + 1.0f) + velocity };
	// 質量が最大の時は変化しない
	if (mass < FLT_MAX - FLT_EPSILON) velocity = velocity_a;
	// 相手の衝突後の速度
	const Vector3 velocity_b{ (collision_vector_velocity_ab - collision_vector_velocity_ba) * (1.0f + restitution) / (opponent->mass / mass + 1.0f) + opponent->velocity };
	// 質量が最大の時は変化しない
	if (opponent->mass < FLT_MAX - FLT_EPSILON) opponent->velocity = velocity_b;

	// めり込み補正
	const Vector3 correction_vector{ direction_vector_ab * abs(penetration) * 0.5f };

	// 質量が最大の時は変化しない
	if (mass < FLT_MAX - FLT_EPSILON) GetActor()->SetWorldPosition(pre_position_a - correction_vector);
	// 質量が最大の時は変化しない
	if (opponent->mass < FLT_MAX - FLT_EPSILON) opponent->GetActor()->SetWorldPosition(pre_position_b + correction_vector);
}

void RigidBody::Collide(RigidBody* opponent, float restitution, const Vector3& normal, float penetration)
{
	// 自身の変化前の位置
	const Vector3 pre_position_a{ GetActor()->GetWorldPosition() };
	// 相手の変化前の位置
	const Vector3 pre_position_b{ opponent->GetActor()->GetWorldPosition() };

	// 自身と相手の衝突方向ベクトル
	const Vector3 direction_vector_ab{ (pre_position_b - pre_position_a).Normalize() };

	// 自身の衝突方向の速度成分
	const Vector3 collision_vector_velocity_ab{ velocity.Dot(direction_vector_ab) * direction_vector_ab };
	// 相手の衝突方向の速度成分
	const Vector3 collision_vector_velocity_ba{ opponent->velocity.Dot(-direction_vector_ab) * (-direction_vector_ab) };

	// 自身の衝突後の速度
	const Vector3 velocity_a{ (collision_vector_velocity_ba - collision_vector_velocity_ab) * (1.0f + restitution) / (mass / opponent->mass + 1.0f) + velocity };
	// 質量が最大の時は変化しない
	if (mass < FLT_MAX - FLT_EPSILON) velocity = velocity_a;
	// 相手の衝突後の速度
	const Vector3 velocity_b{ (collision_vector_velocity_ab - collision_vector_velocity_ba) * (1.0f + restitution) / (opponent->mass / mass + 1.0f) + opponent->velocity };
	// 質量が最大の時は変化しない
	if (opponent->mass < FLT_MAX - FLT_EPSILON) opponent->velocity = velocity_b;

	// めり込み補正
	const Vector3 correction_vector{ normal * abs(penetration) };

	// 質量が最大の時は変化しない
	if (mass < FLT_MAX - FLT_EPSILON) GetActor()->SetWorldPosition(pre_position_a - correction_vector);
	// 質量が最大の時は変化しない
	if (opponent->mass < FLT_MAX - FLT_EPSILON) opponent->GetActor()->SetWorldPosition(pre_position_b + correction_vector);
}

void RigidBody::MoveToPoint(const Vector3& goal_point, float displacement)
{


	return;
}

void RigidBody::AddMoveDirectionForce(const Vector3& move_direction, float move_speed, float max_speed)
{
	// 速度ベクトルの長さが最大速度より小さいとき
	if (velocity.Length() < max_speed)
	{
		AddVelocity(move_direction * move_speed);
	}
}

bool RigidBody::OnGround() const
{
	return on_ground;
}

bool RigidBody::IsCollideWall() const
{
	return is_collide_wall;
}

void RigidBody::SuddenStop(float interpolated)
{
	velocity = velocity.Lerp(Vector3::ZeroVector(), (std::min)(interpolated, 1.0f));
}

void RigidBody::SlidingWall(
	const Vector3& ray_start,
	const Vector3& collide_position,
	const Vector3& wall_normal)
{
	const Vector3 normal1{ wall_normal.Normalize() };
	const Vector3 vec1{ collide_position - ray_start }; 

	velocity = vec1 - vec1.Dot(normal1) * normal1;

}

void RigidBody::UpdateFriction(float dynamic_friction)
{
	// 動いていない(速度が０)の時は処理を行わない
	if (velocity.IsZero())
	{
		return;
	}

	// -向き * 質量 * 動摩擦係数
	AddForce(-velocity * mass * dynamic_friction);
}

void RigidBody::Integrate(float elapsed_time)
{
	std::shared_ptr<Actor> actor{ GetActor() };
	// 加速度の更新
	acceleration = resultant * (1 / mass);
	// 速度の更新
	velocity += acceleration * elapsed_time;

	if (elapsed_time > 0.1f)
	{
		// HACK : ゲームが重かったり固まる時間が発生してelapsed_timeの値が大きくなって
		//        移動量がおかしくなることを抑制する
		velocity = {};
	}

	// 地形との衝突
	is_collide_wall = GameSys.GetStageManager()->IntersectModelVSTerrain(GetActor().get(), 0.05f);

	// 位置の更新
	actor->SetWorldPosition(actor->GetWorldPosition() + velocity * elapsed_time);

	// 合力を初期化
	resultant = {};
}
