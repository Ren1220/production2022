#include "SphereCast.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/Model/GLTFModelData.h"
#include "Debug/DebugPrimitive.h"
#include "IntersectUser/Intersect.h"


/* --- ��` ---- */

bool IntersectUser::IntersectSphereCastVSTriangleEST(
	const Vector3& start,
	const Vector3& end,
	const float radius,
	const Vector3 triangle_position[3],
	HitResult::ResultRayVSTriangle& result)
{
	// �O�p�`�@�Ɓ@�n�_�ɂ����鋅�@���������Ă���ꍇ�́Afalse ��Ԃ��B�B
	if (IntersectSphereVSTriangle(start, radius, triangle_position, result.intersect_position))
	{
		return false;
	}

	Vector3 ray_dir{ end - start };
	Vector3 ab{ triangle_position[1] - triangle_position[0] };
	Vector3 ac{ triangle_position[2] - triangle_position[0] };
	Vector3 cross1{ ab.Cross(ac) }; // �O�p�`�̖@��

	Vector3 vec1{ start - end }; // �I�_����n�_�ւ̃x�N�g��

	// �O�p�`�̖@���@�Ɓ@�I�_�[�n�_�x�N�g���@�́@���ϒl
	float dot1{ cross1.Dot(vec1) };

	// �\�ʂ���������Ă��Ȃ���� false ��Ԃ��B
	if (dot1 <= 0.0f)
	{
		return false;
	}

	// ���C�@�Ɓ@�O�p�`�̖@���@���@�����ȏꍇ�A���C�@�Ɓ@�O�p�`�͕��s�Ȃ̂ŁAfalse ��Ԃ��B
	if (fabsf(dot1) <= FLT_EPSILON) // ����Ȃ��O�ɋ߂��ꍇ
	{
		return false;
	}

	// ���C�̎n�_�@���@�O�p�`�̖@���@���@��ɋ��\�ʂ̍ŒZ�_�@�Ɉړ�������B
	// https://www.geogebra.org/calculator/nsegxbkk
	// �O�p�`�̐��K���ς݋t�@���@����@���̔��a���@���炵���x�N�g���B
	Vector3 fix_vec{ (-cross1).Normalize() * radius };
	Vector3 start_fix{ start + fix_vec }; // �␳��̎n�_

	// ���_�O�@�Ɓ@�␳��̎n�_�@�̃x�N�g��
	Vector3 vec3{ start_fix - triangle_position[0] };
	float dot2 = cross1.Dot(vec3);

	// �O�p�`�̖@���@�Ɓ@�␳��̃��C�@�������������Ă��邩�A
	// ���C�̒�����
	if (dot2 >= 0.0f && dot2 < dot1)
	{
		// ���C�@�Ɓ@�O�p�`���܂ޕ��ʂ̌�_�@�̎Z�o

		Vector3 cross2{ vec1.Cross(vec3) };

		float dot3{ ac.Dot(cross2) };
		float dot4{ -ab.Dot(cross2) };
		dot2 /= dot1;
		result.intersect_position = start_fix + ray_dir * dot2;

		if (dot3 >= 0.0f && dot3 <= dot1 && dot4 > 0.0f && dot3 + dot4 <= dot1)
		{
			// ���C�@�Ɓ@�O�p�`���܂ޕ��ʂ̌�_�@���O�p�`�̓���
			result.distance = ray_dir.Length() * dot2;
			result.triangle_normal = cross1;
			result.triangle_vertices[0] = triangle_position[0];
			result.triangle_vertices[1] = triangle_position[1];
			result.triangle_vertices[2] = triangle_position[2];
			return true;
		}
		else
		{
			// ���C�@�Ɓ@�O�p�`���܂ޕ��ʂ̌�_�@���O�p�`�̊O��

			// ��_�ɍł��߂��O�p�`�����̓_�@�����߂�
			Vector3 near_position{};
			ClosestPositionPointTriangle(result.intersect_position, triangle_position, near_position);

			HitResult::ResultRayVSSphere result0{  };
			// ��_�Ɉ�ԋ߂��O�p�`�����̓_����A�t�����̃��C�𔭎˂��A
			// ���C�̎n�_�ɂ��鋅�̂ƌ������邩�m�F
			// ���������ꍇ�A
			// ��_�Ɉ�ԋ߂��O�p�`�����̓_���X�t�B�A�L���X�g�̎O�p�`�Ƃ̌�_�ɂȂ�
			if (IntersectRayVSSphere(
				near_position,
				near_position - ray_dir,
				start,
				radius,
				result0))
			{
				result.intersect_position = near_position;
				result.distance = result0.distance;
				result.triangle_normal = cross1; 
				result.triangle_vertices[0] = triangle_position[0];
				result.triangle_vertices[1] = triangle_position[1];
				result.triangle_vertices[2] = triangle_position[2];
				return true;
			}
		}
	}

	return false;
}
