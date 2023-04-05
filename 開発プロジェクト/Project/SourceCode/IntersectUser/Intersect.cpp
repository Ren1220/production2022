#include "Intersect.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Debug/DebugPrimitive.h"

/* --- ��` ---- */

bool IntersectUser::ClosestPositionPointTriangle(
	const Vector3& point,
	const Vector3 triangle_position[3],
	Vector3& near_position,
	Vector3& near_triangle_position0,
	Vector3& near_triangle_position1)
{
	const Vector3 vec01{ triangle_position[1] - triangle_position[0] };
	const Vector3 vec02{ triangle_position[2] - triangle_position[0] };

	// point��triangle_position[0]�̊O���̒��_�̈�ɑ��݂��邩
	const Vector3 vec0P{ point - triangle_position[0] };
	float d1{ vec01.Dot(vec0P) };
	float d2{ vec02.Dot(vec0P) };

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// �ŋߓ_��triangle_position[0]�A�����͂��Ă��Ȃ�
		near_position = triangle_position[0];
		near_triangle_position0 = triangle_position[0];
		return false;
	}

	// point��triangle_position[1]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
	const Vector3 vec1P{ point - triangle_position[1] };
	float d3 { vec01.Dot(vec1P) };
	float d4 { vec02.Dot(vec1P) };

	if (d3 >= 0.0f && d4 <= d3)
	{
		// �ŋߓ_��triangle_position[1]�A�����͂��Ă��Ȃ�
		near_position = triangle_position[1];
		near_triangle_position0 = triangle_position[1];
		return false;
	}

	// point��triangle_position[0]��triangle_position[1]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float scalar3_01 = d1 * d4 - d3 * d2;

	if (scalar3_01 <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		// �ŋߓ_��triangle_position[0], triangle_position[1]�A�����͂��Ă��Ȃ�
		float t = d1 / (d1 - d3);
		near_position = triangle_position[0] + vec01 * t;
		near_triangle_position0 = triangle_position[0];
		near_triangle_position1 = triangle_position[1];
		return false;
	}

	// point��triangle_position[2]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
	const Vector3 vec2P{ point - triangle_position[2] };
	float d5 { vec01.Dot(vec2P) };
	float d6 { vec02.Dot(vec2P) };

	if (d6 >= 0.0f && d5 <= d6)
	{
		near_position = triangle_position[2];
		near_triangle_position0 = triangle_position[2];
		return false;
	}

	// point��triangle_position[0]��triangle_position[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float scalar3_02 = d5 * d2 - d1 * d6;

	if (scalar3_02 <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float t = d2 / (d2 - d6);
		near_position = triangle_position[0] + vec02 * t;
		near_triangle_position0 = triangle_position[0];
		near_triangle_position1 = triangle_position[2];
		return false;
	}

	// point��triangle_position[1]��triangle_position[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
	float scalar3_12 = d3 * d6 - d5 * d4;

	if (scalar3_12 <= 0.0f && d4 >= d3 && d5 >= d6)
	{
		float t = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		near_position = triangle_position[1] + (triangle_position[2] - triangle_position[1]) * t;
		near_triangle_position0 = triangle_position[1];
		near_triangle_position1 = triangle_position[2];
		return false;
	}

	// �����܂ł���΁AnearPos�͎O�p�`�̓����ɂ���
	float denom = 1.0f / (scalar3_01 + scalar3_02 + scalar3_12);
	float t01 = scalar3_02 * denom;
	float t02 = scalar3_01 * denom;
	near_position = triangle_position[0] + vec01 * t01 + vec02 * t02;
	return true;
}

bool IntersectUser::ClosestPositionPointTriangle(
	const Vector3& point, 
	const Vector3 triangle_position[3], 
	Vector3& near_poisiton)
{
	Vector3 dummy1;
	Vector3 dummy2;

	return IntersectUser::ClosestPositionPointTriangle(point, triangle_position, near_poisiton, dummy1, dummy2);
}

bool IntersectUser::IntersectSphereVSTriangle(
	const Vector3& sphere_position, 
	const float radius, 
	const Vector3 triangle_position[3],
	Vector3& near_position)
{
	// ���̒��S�_�@����@�ł��߂����_�@�����߂�
	ClosestPositionPointTriangle(sphere_position, triangle_position, near_position);
	// ���̒��S�_�Ƌ��߂��ŋߓ_�Ƃ̋����̕����@�Ɓ@���a�̓��@���r���A
	// �������Ă��邩���肷��B
	return ((sphere_position - near_position).LengthSquare() <= (radius * radius));
}

bool IntersectUser::IntersectRayVSTriangle(
	const Vector3& ray_start, 
	const Vector3& ray_direction, 
	const float ray_length, 
	const Vector3 triangle_position[3],
	HitResult::ResultRayVSTriangle& result)
{
	const Vector3 ab{ triangle_position[1] - triangle_position[0] };
	const Vector3 ac{ triangle_position[2] - triangle_position[0] };
	const Vector3 cross1{ ab.Cross(ac) };
	// ���C�̎n�_�@����@�I�_�@�ւ̃x�N�g��
	const Vector3 qp{ ray_start - (ray_start + ray_direction * ray_length) };

	const float dot1{ cross1.Dot(qp) };

	// �\�ʂ�������@���Ă��邩
	if (dot1 > 0.0f)
	{
		// ���C�@�Ɓ@�@���@�������łȂ����B�����̎��A���C�@�Ɓ@�O�p�`�@�͕��s�B
		if (fabsf(dot1) > FLT_EPSILON)
		{
			Vector3 ap{ ray_start - triangle_position[0] };
			float dot2{ cross1.Dot(ap) };

			// ���C�̌����@�Ɓ@���C�̒����@
			if (dot2 >= 0.0f && dot2 < dot1)
			{
				// �n�_�@����@���_�@�ւ̃x�N�g���Ɓ@�n�_�@����@�I�_�@�ւ̃x�N�g���̊O��
				Vector3 cross2{ qp.Cross(ap) }; 
				float dot3{ ac.Dot(cross2) };

				if (dot3 >= 0.0f && dot3 <= dot1)
				{
					float dot4{ -ab.Dot(cross2) };
					if (dot4 > 0.0f && dot3 + dot4 <= dot1)
					{
						result.intersect_position = ray_start + ray_direction * (ray_length * dot2 / dot1);
						result.distance = (ray_length * dot2 / dot1);
						result.triangle_normal = cross1;
						result.triangle_vertices[0] = triangle_position[0];
						result.triangle_vertices[1] = triangle_position[1];
						result.triangle_vertices[2] = triangle_position[2];
						return true;
					}
				}
			}
		}
	}

	result = {};
	return false;
}

bool IntersectUser::IntersectRayVSSphere(
	const Vector3& start, 
	const Vector3& end, 
	const Vector3& sphere_position, 
	const float radius, 
	HitResult::ResultRayVSSphere& result)
{
	Vector3 ray_dir{ end - start };
	Vector3 ray_dir_norm{ ray_dir.Normalize() };
	Vector3 vec0{ sphere_position - start };
	// �n�_�@����@���̒��S�_�ւ̃x�N�g���@���@���C�̃x�N�g���@�Ɏˉe����B
	float proj{ vec0.Dot(ray_dir_norm) }; 
	float dist_sq{ vec0.LengthSquare() - proj };

	// ���̔��a�@���@�n�_�Ƌ��̒��S�_�̋����@���傫����
	if (dist_sq < radius * radius)
	{
		// ��r�����l�������@�Ȃ̂ŕ��������s���B
		result.distance = proj - sqrtf(radius * radius - dist_sq);
		// �����@���@���̒l�@���@���C�̒����@���Z����
		if (result.distance > 0.0f && result.distance < ray_dir.Length())
		{
			result.intersect_position = start + ray_dir_norm * result.distance;
			return true;
		}
	}

	result.intersect_position = {};
	result.distance = 0.0f;

	return false;
}

bool IntersectUser::IntersectSphereVSCapsule(
	const Vector3& s_pos, 
	const float s_rad, 
	const Vector3& c_start, 
	const Vector3& c_end, 
	const float c_rad, 
	float& dist)
{
	Vector3 c_vec{ c_end - c_start };
	Vector3 c_dir{ c_vec.Normalize() };

	Vector3 vec{ s_pos - c_start };

	float dot{ vec.Dot(c_dir) };

	Vector3 c_near{};

	// �ˉe�����}�C�i�X�������́A�~���̒����𒴂���ꍇ�␳��������K�v������
	if (dot < 0.0f)
	{
		c_near = c_start;
	}
	else if (dot * dot > c_vec.LengthSquare())
	{
		c_near = c_end;
	}
	// ���Ȃ��ꍇ�͎ˉe�����ʒu���ŋߓ_
	else
	{
		c_near = c_start + dot * c_dir;
	}

	dist = (s_pos - c_near).Length();

	if (c_rad + s_rad > dist)
	{
		return true;
	}

	return false;
}

bool IntersectUser::IntersectCapsuleVSCapsule(
	const Vector3& a_start, 
	const Vector3& a_end, 
	const float a_rad, 
	const Vector3& b_start, 
	const Vector3& b_end, 
	const float b_rad)
{
	Vector3 dummy;
	return IntersectCapsuleVSCapsule(a_start, a_end, a_rad, b_start, b_end, b_rad, dummy, dummy);
}

bool IntersectUser::IntersectCapsuleVSCapsule(
	const Vector3& a_start, 
	const Vector3& a_end, 
	const float a_rad, 
	const Vector3& b_start, 
	const Vector3& b_end, 
	const float b_rad, 
	Vector3& out_a_near, 
	Vector3& out_b_near)
{
	Vector3 a_vec{ a_end - a_start };
	Vector3 b_vec{ b_end - b_start };

	Vector3 ab_vec{ a_start - b_start };

	float a{ a_vec.Dot(a_vec) };
	float b{ a_vec.Dot(b_vec) };
	float c{ a_vec.Dot(ab_vec) };
	float e{ b_vec.Dot(b_vec) };
	float f{ b_vec.Dot(ab_vec) };

	float denom{ a * e - b * b };

	float a_t{}, b_t{};

	bool is_point[2]{ a < FLT_EPSILON, e < FLT_EPSILON };

	// �����_
	if (is_point[0] && is_point[1])
	{
		a_t = 0;
		b_t = 0;
	}
	// a�̂ݓ_
	else if (is_point[0])
	{
		a_t = 0;
		b_t = f / e;
	}
	// b�̂ݓ_
	else if (is_point[1])
	{
		a_t = -c / a;
		b_t = 0;
	}
	else
	{
		a_t = (b * f - c * e) / denom;
		b_t = (a * f - b * c) / denom;
	}

	// ���ꂪ�[���̏ꍇ��̐��������s�ł���
	if (denom < FLT_EPSILON && denom > -FLT_EPSILON)
	{
		a_t = 0;
	}

	// �Z�o��������t��������Ɏ��߂�
	a_t = std::clamp(a_t, 0.0f, 1.0f);

	// ����b�ɍŒZ�_a����ˉe���čŒZ�_b�����߂� 
	b_t = (a_t * b + f) / e;

	// ����b�̊O�������ꍇ�A����b�̒[���ŒZ�_���Z�o���A����a��̍ŒZ�_���Čv�Z
	if (b_t > 1.0f)
	{
		b_t = 1.0f;
		a_t = (b - c) / a;
	}
	else if (b_t < 0.0f)
	{
		b_t = 0.0f;
		a_t = -c / a;
	}

	// �Z�o��������t��������Ɏ��߂�
	a_t = std::clamp(a_t, 0.0f, 1.0f);

	// ��̐����̍ŒZ�_����̋����Ɠ�̃J�v�Z���̔��a���r����
	out_a_near = a_start + a_vec * a_t;
	out_b_near = b_start + b_vec * b_t;

	if ((out_b_near - out_a_near).LengthSquare() <= (a_rad + b_rad) * (a_rad + b_rad))
	{
		return true;
	}

	return false;
}

