#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Arithmetic/Arithmetic.h"

#include "../Component/Component.h"

/* --- �錾 ---- */

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
	/// �̓x�N�g�������͂���
	/// </summary>
	void AddForce(const Vector3& force) { resultant += force; }

	/// <summary>
	/// ���ʂ̎擾
	/// </summary>
	float GetMass() const { return mass; }

	/// <summary>
	/// ���x�̎擾
	/// </summary>
	const Vector3& GetVelocity() const { return velocity; }

	/// <summary>
	/// �����x�̎擾
	/// </summary>
	const Vector3& GetAcceleration() const { return acceleration; }

	/// <summary>
	/// ���x�̉��Z
	/// </summary>
	void AddVelocity(const Vector3& velocity) { this->velocity += velocity; }

	/// <summary>
	/// �����x�̉��Z
	/// </summary>
	void AddAcceleration(const Vector3& acceleration) { this->acceleration += acceleration; }

	/// <summary>
	/// ���̓��m�̏Փ�
	/// </summary>
	/// <param name="opponent">�Փ˂��鑊��</param>
	/// <param name="restitution">�����W��</param>
	/// <param name="penetration">�H�����ݗ�</param>
	void Collide(RigidBody* opponent, float restitution, float penetration);
	void Collide(RigidBody* opponent, float restitution, const Vector3& normal, float penetration);

	/// <summary>
	/// �ړI�n�܂ł̈ړ�
	/// </summary>
	/// <param name="goal_point">�ړI�n</param>
	/// <param name="displacement">�ψ�</param>
	void MoveToPoint(const Vector3& goal_point, float displacement);

	/// <summary>
	/// �ړ����x�ɉ������͂��ړ���������ɉ�����
	/// </summary>
	/// <param name="move_direction"></param>
	/// <param name="move_speed"></param>
	/// <param name="max_speed"></param>
	void AddMoveDirectionForce(const Vector3& move_direction, float move_speed, float max_speed);

	bool OnGround() const;

	bool IsCollideWall() const;

	/// <summary>
	/// �}��~
	/// </summary>
	void SuddenStop(float interpolated);

	/// <summary>
	/// ���݂̈ړ��ʂ��A�ǂ��肵���ۂ̈ړ��ʂɁ@�ϊ��@����B
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
	/// ���C�͂̌v�Z
	/// </summary>
	/// <param name="dynamic_friction"></param>
	void UpdateFriction(float dynamic_friction = 2.0f);

	/// <summary>
	/// ���͂ɂ��ړ��l�̌v�Z
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void Integrate(float elapsed_time);

private:

	bool is_effect_gravity;
	bool on_ground;

	bool is_collide_wall;

	float dynamic_friction;

	const float mass;				 // ����
	Vector3 velocity;		 // ���x
	Vector3 acceleration;  // �����x
	Vector3 resultant;	 // ����

	static constexpr float gravitational_acceleration{ -9.80665f };  // �d�͉����x
};
