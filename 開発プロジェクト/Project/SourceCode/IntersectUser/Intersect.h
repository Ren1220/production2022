#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Arithmetic/Arithmetic.h"

/* --- �O���錾 ---- */



/* --- �錾 ---- */


namespace IntersectUser
{
	namespace HitResult
	{
		struct ResultRayVSSphere
		{
			Vector3 intersect_position;
			float distance;
		};

		struct ResultRayVSTriangle
		{
			Vector3 intersect_position;
			Vector3 triangle_normal;
			float distance;
			Vector3 triangle_vertices[3];
		};
	}

	/// <summary>
	/// �C�ӂ̓_�@�́@�R���_���ɂ�����ŋߓ_�@�����߂�
	/// </summary>
	/// <param name="near_triangle_poisiton0">���_�̈�ɔC�ӂ̓_�����݂��钸�_</param>
	/// <param name="near_triangle_poisiton1">���_�̈�ɔC�ӂ̓_�����݂��钸�_</param>
	/// <returns>�O���_�̓����ɔC�ӂ̓_�����݂��邩</returns>
	bool ClosestPositionPointTriangle(
		const Vector3& point,
		const Vector3 triangle_position[3],
		Vector3& near_poisiton,
		Vector3& near_triangle_poisiton0,
		Vector3& near_triangle_poisiton1);

	/// <summary>
	/// �C�ӂ̓_�@�́@�R���_���ɂ�����ŋߓ_�@�����߂�
	/// </summary>
	/// <returns>�O���_�̓����ɔC�ӂ̓_�����݂��邩</returns>
	bool ClosestPositionPointTriangle(
		const Vector3& point,
		const Vector3 triangle_position[3],
		Vector3& near_poisiton);
	
	/// <summary>
	/// ���̒��S�_����ł��߂����_�@�����߁A���̒��S�_�Ƃ̋����@�Ɓ@���̔��a�@���r���A
	/// �������Ă��邩���肷��B
	/// </summary>
	bool IntersectSphereVSTriangle(
		const Vector3& sphere_position,
		const float radius,
		const Vector3 triangle_position[3],
		Vector3& near_position);

	bool IntersectRayVSTriangle(
		const Vector3& ray_start,
		const Vector3& ray_direction,
		const float ray_length,
		const Vector3 triangle_position[3],
		HitResult::ResultRayVSTriangle& near_position);

	/// <summary>
	/// ���̒��S�_�ƃ��C�̎n�_�̃x�N�g�������C�Ɏˉe���āA
	/// ���̔��a�Ǝˉe���ċ��߂��������r���A�������Ă��邩���肷��B
	/// </summary>
	bool IntersectRayVSSphere(
		const Vector3& start,
		const Vector3& end,
		const Vector3& sphere_position,
		const float radius,
		HitResult::ResultRayVSSphere& result);

	/// <summary>
	/// ���̒��S�_�ւ̃J�v�Z���̍ŋߓ_�����߁A
	/// �ŋߓ_���狅�ւ̋����ƁA�e���a�̘a���r����B
	/// </summary>
	/// <returns></returns>
	bool IntersectSphereVSCapsule(
		const Vector3& s_pos,
		const float s_rad,
		const Vector3& c_start,
		const Vector3& c_end,
		const float c_rad,
		float& dist);

	/// <summary>
	/// �J�v�Z�����m�̊e������̍ŋߓ_�����߁A
	/// �ŋߓ_���m�̋����ƁA�e���a�̘a���r����B
	/// </summary>
	bool IntersectCapsuleVSCapsule(
		const Vector3& a_start,
		const Vector3& a_end,
		const float a_rad,
		const Vector3& b_start,
		const Vector3& b_end,
		const float b_rad);

	/// <summary>
	/// �J�v�Z�����m�̊e������̍ŋߓ_�����߁A
	/// �ŋߓ_���m�̋����ƁA�e���a�̘a���r����B
	/// </summary>
	bool IntersectCapsuleVSCapsule(
		const Vector3& a_start,
		const Vector3& a_end,
		const float a_rad,
		const Vector3& b_start,
		const Vector3& b_end,
		const float b_rad,
		Vector3& out_a_near,
		Vector3& out_b_near);
}
