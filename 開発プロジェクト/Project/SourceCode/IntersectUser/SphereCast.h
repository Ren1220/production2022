#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Intersect.h"

/* --- �O���錾 ---- */

class GLTFModelData; 


/* 
	--- �T�v ---- 

	�擾�ł���l : 
		���C�̔��˓_�ł���A���̂̒��S����A���̂��O�p�`�ɓ����������̋��̂̒��S�܂ł̋����B
		���̂ƎO�p�`�̐ړ_�B

	
	�� ���C�L���X�g�Ƃ͈قȂ�A��_���n�_����̒�����ɂ���Ƃ͌���Ȃ�

	�����b�g�F
		���C��������Ȃ��Ă����̂����������ꍇ�Ɍ�_���Ƃ��B
		�O�p�`�ɕ��s�ȃ��C�ł����������_�̎Z�o���ł���B
		�A���������b�V���̌��Ԕ������������Ȃ�
		���C�̌�_�Ƃ͕ʂ̎O�p�`�ɂ߂荞�ނ�������ȃP�[�X�̏������A���a�����������C��p���āA
�@�@�@	   �ړ_�i�Γ_�j�x�[�X�ŎZ�o����邽�߁A�߂荞�ݏ������i��{�I�Ɂj�s�v�B
	
	�f�����b�g�F
		���ʂ̃��C�L���X�g�Ɣ�ׂāA�������d���B
		�������A�S�{���C�Ɣ�r����Ɗ��S�łłقړ����AEST�ł��ƂR���O�ギ�炢�����B
		�Ȃ��A�S�{���C�@�͌�������ƌ�_�Z�o�͗e�Ղ����A�ړ_�̎Z�o�͂��Ȃ����A
		�܂��ړ_�Z�o�̏������ׂ�߂荞�݉��������́A��L��r�Ɋ܂܂�Ă��Ȃ��B

	EST : �����덷����
*/


/* --- �錾 ---- */

namespace IntersectUser
{

	/// <summary>
	/// �ȈՔŃX�t�B�A�L���X�g�B�ڍׂ�Header�ɂāB
	/// </summary>
	/// <param name="start">���C�̎n�_</param>
	/// <param name="end">���C�̏I�_</param>
	/// <param name="radius">�X�t�B�A�L���X�g�̔��a</param>
	/// <param name="triangle_position">�L���X�g����|���S���̒��_</param>
	/// <param name="result">�L���X�g����</param>
	/// <param name="is_front_conuter_clock_wise">�|���S���̒��_�̏���</param>
	/// <returns></returns>
	bool IntersectSphereCastVSTriangleEST(
		const Vector3& start,
		const Vector3& end,
		const float radius,
		const Vector3 triangle_position[3],
		HitResult::ResultRayVSTriangle& result);
}
