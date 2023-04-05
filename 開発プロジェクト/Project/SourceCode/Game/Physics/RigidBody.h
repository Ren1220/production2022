#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Arithmetic/Arithmetic.h"

#include "../Component/Component.h"

/* --- 宣言 ---- */

class RigidBody : public Component
{
public:

	RigidBody(float mass = FLT_MAX, bool is_effect_gravity = false);
	~RigidBody();

	void Start();
	void Update(float elapsed_time);
	void ImGuiRender();

public:

	/// <summary>
	/// 力ベクトルを合力する
	/// </summary>
	void AddForce(const Vector3& force) { resultant += force; }

	/// <summary>
	/// 質量の取得
	/// </summary>
	float GetMass() const { return mass; }

	/// <summary>
	/// 速度の取得
	/// </summary>
	const Vector3& GetVelocity() const { return velocity; }

	/// <summary>
	/// 加速度の取得
	/// </summary>
	const Vector3& GetAcceleration() const { return acceleration; }

	/// <summary>
	/// 速度の加算
	/// </summary>
	void AddVelocity(const Vector3& velocity) { this->velocity += velocity; }

	/// <summary>
	/// 加速度の加算
	/// </summary>
	void AddAcceleration(const Vector3& acceleration) { this->acceleration += acceleration; }

	/// <summary>
	/// 物体同士の衝突
	/// </summary>
	/// <param name="opponent">衝突する相手</param>
	/// <param name="restitution">反発係数</param>
	/// <param name="penetration">食い込み量</param>
	void Collide(RigidBody* opponent, float restitution, float penetration);
	void Collide(RigidBody* opponent, float restitution, const Vector3& normal, float penetration);

	/// <summary>
	/// 目的地までの移動
	/// </summary>
	/// <param name="goal_point">目的地</param>
	/// <param name="displacement">変位</param>
	void MoveToPoint(const Vector3& goal_point, float displacement);

	/// <summary>
	/// 移動速度に応じた力を移動する向きに加える
	/// </summary>
	/// <param name="move_direction"></param>
	/// <param name="move_speed"></param>
	/// <param name="max_speed"></param>
	void AddMoveDirectionForce(const Vector3& move_direction, float move_speed, float max_speed);

	bool OnGround() const;

	bool IsCollideWall() const;

	/// <summary>
	/// 急停止
	/// </summary>
	void SuddenStop(float interpolated);

	/// <summary>
	/// 現在の移動量を、壁ずりした際の移動量に　変換　する。
	/// </summary>
	/// <param name="ray_start"></param>
	/// <param name="collide_position"></param>
	/// <param name="wall_normal"></param>
	void SlidingWall(
		const Vector3& ray_start,
		const Vector3& collide_position,
		const Vector3& wall_normal);

	void SetDynamicFriction(float f = 10.0f)
	{
		dynamic_friction = f;
	}

private:

	/// <summary>
	/// 摩擦力の計算
	/// </summary>
	/// <param name="dynamic_friction"></param>
	void UpdateFriction(float dynamic_friction = 2.0f);

	/// <summary>
	/// 合力による移動値の計算
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Integrate(float elapsed_time);

private:

	bool is_effect_gravity;
	bool on_ground;

	bool is_collide_wall;

	float dynamic_friction;

	const float mass;				 // 質量
	Vector3 velocity;		 // 速度
	Vector3 acceleration;  // 加速度
	Vector3 resultant;	 // 合力

	static constexpr float gravitational_acceleration{ -9.80665f };  // 重力加速度
};
