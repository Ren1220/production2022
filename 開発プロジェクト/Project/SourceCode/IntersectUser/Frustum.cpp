#include "Frustum.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Debug/DebugPrimitive.h"

/* --- ��` ---- */

void Frustum::Update(const Matrix& view, const Matrix& proj)
{
	// �r���[�v���W�F�N�V����
	Matrix view_proj{ view * proj };
	// �r���[�v���W�F�N�V�����t�s��
	Matrix ivp{ view_proj.Inverse() };

	// �r���[�v���W�F�N�V�������̒��_�Z�o�p�ʒu�x�N�g��
	constexpr Vector3 vert[8]
	{
		// near
		{ -1.0f, -1.0f, 0.0f },	// ����
		{  1.0f, -1.0f, 0.0f },	// �E��
		{  1.0f,  1.0f, 0.0f },	// �E��
		{ -1.0f,  1.0f, 0.0f },	// ����

		// far
		{ -1.0f, -1.0f, 1.0f },	// ����
		{  1.0f, -1.0f, 1.0f },	// �E��
		{  1.0f,  1.0f, 1.0f },	// �E��
		{ -1.0f,  1.0f, 1.0f }, // ����
	};

	// �r���[�v���W�F�N�V�����s��̋t�s���p���āA�e���_���Z�o����

	for (int i = 0; i < 8; ++i)
	{
		vertices.pos[i] = vert[i].TransformCoordinate(ivp);
	}

	// 0:������, 1:�E����, 2:������, 3:�㑤��, 4:������, 5:��O����
	// �S�Ă̖ʂ̖@���͓����������悤�ɐݒ肷��
	// ���K��������
	frustum[0].normal = Vector3( ivp._14 + ivp._11,  ivp._24 + ivp._21,  ivp._34 + ivp._31).Normalize();
	frustum[1].normal = Vector3( ivp._14 - ivp._11,  ivp._24 - ivp._21,  ivp._34 - ivp._31).Normalize();
	frustum[2].normal = Vector3( ivp._14 + ivp._12,  ivp._24 + ivp._22,  ivp._34 + ivp._32).Normalize();
	frustum[3].normal = Vector3( ivp._14 - ivp._12,  ivp._24 - ivp._22,  ivp._34 - ivp._32).Normalize();
	frustum[4].normal = Vector3(-ivp._14 - ivp._13, -ivp._24 - ivp._23, -ivp._34 - ivp._33).Normalize();
	frustum[5].normal = Vector3(-ivp._14 + ivp._13, -ivp._24 + ivp._23, -ivp._34 + ivp._33).Normalize();

	// ���_����̋���
	frustum[0].distance = vertices.near_z[0].Dot(frustum[0].normal);
	frustum[1].distance = vertices. far_z[2].Dot(frustum[1].normal);
	frustum[2].distance = vertices.near_z[0].Dot(frustum[2].normal);
	frustum[3].distance = vertices. far_z[2].Dot(frustum[3].normal);
	frustum[4].distance = vertices. far_z[2].Dot(frustum[4].normal);
	frustum[5].distance = vertices.near_z[0].Dot(frustum[5].normal);
}

bool Frustum::IntersectAABB(const Vector3& min, const Vector3& max)
{
	constexpr auto calc_min_max = 
		[](float norm_param, 
		   float radius, 
		   float& nega,
		   float& posi)
	{
		if (norm_param >= 0)
		{
			posi += radius;
			nega -= radius;
		}
		else
		{
			posi -= radius;
			nega += radius;
		}
	};

	AABB aabb(min, max);
	collision_state = 0;

	for (int i = 0; i < 6; ++i)
	{
		auto& frus{ frustum[i] };

		// �e���ʂ̖@���̐�����p����AABB�̂W���_�̒�����ŋߓ_�ƍŉ��_�����߂�
		Vector3 nega = aabb.center;	// �ŋߓ_
		Vector3 posi = aabb.center;	// �ŉ��_

		// �e�����̍ŋߓ_�A�ŉ��_
		calc_min_max(frus.normal.x, aabb.radius.x, nega.x, posi.x);
		calc_min_max(frus.normal.y, aabb.radius.y, nega.y, posi.y);
		calc_min_max(frus.normal.z, aabb.radius.z, nega.z, posi.z);

		float d_nega{ nega.Dot(frus.normal) };
		float d_posi{ posi.Dot(frus.normal) };

		if (d_posi < frus.distance && d_nega < frus.distance)
		{
			collision_state = 2;
			break;
		}
		else if (d_posi <= frus.distance && d_nega > frus.distance)
		{
			collision_state = 1;
		}
		else if (d_posi > frus.distance && d_nega <= frus.distance)
		{
			collision_state = 1;
		}
	}

	// ������̌�둤��AABB�����݂̃A���S���Y���ł͏�����������\��������ׁA���̔�����s���B
	// AABB�̌��ʂ̒��S�_�Ɩ@�������A������̂W���_���ׂĂ��\���ɂ���Ό������Ă��Ȃ��B
	if (collision_state = 1)
	{
		Vector3 fface{ aabb.center.x,  aabb.center.y, aabb.center.z - aabb.radius.z };
		Vector3 l_t_f{ aabb.center.x - aabb.radius.x, aabb.center.y + aabb.radius.y, aabb.center.z - aabb.radius.z };
		Vector3 r_t_f{ aabb.center.x + aabb.radius.x, aabb.center.y + aabb.radius.y, aabb.center.z - aabb.radius.z };
		Vector3 r_b_f{ aabb.center.x + aabb.radius.x, aabb.center.y - aabb.radius.y, aabb.center.z - aabb.radius.z };

		// ���ʂ̖@��
		Vector3 n{ ((l_t_f - r_t_f).Cross(r_b_f - l_t_f)).Normalize() };

		// ���ʂ̖@���Ɗe���_�̓��ϒl������������߁A�\��������s���B
		for (int i = 0; i < 8; ++i)
		{
			// �ʂ̒��S�_����e���_�ւ̐��K���x�N�g��
			Vector3 vecn{ (fface - vertices.pos[i]).Normalize() };

			float d{ n.Dot(vecn) };

			if (d > 0)
			{
				return true;
			}
		}

		return false;
	}
	else if (collision_state == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}

void Frustum::DrawDebugPrimitive(const Vector4& color)
{
	auto& debug{ DebugPrimitive::GetInstance() };

	debug.DrawLine(vertices.near_z[0], vertices.far_z[0], color);
	debug.DrawLine(vertices.near_z[1], vertices.far_z[1], color);
	debug.DrawLine(vertices.near_z[2], vertices.far_z[2], color);
	debug.DrawLine(vertices.near_z[3], vertices.far_z[3], color);
}

void Frustum::AABB::CalcParam(
	const Vector3& min,
	const Vector3& max)
{
	radius = max - min;
	center = (min + max) / 2;
}
