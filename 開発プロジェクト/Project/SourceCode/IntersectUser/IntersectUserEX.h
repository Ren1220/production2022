#pragma once

// ���̃Q�[���p�̂��܂�ėp�I�łȂ�����

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "IntersectModel.h"
#include "Game/Stage/Terrain/TileData.h"

/* --- �O���錾 ---- */



/* --- �錾 ---- */


namespace IntersectUser
{
	bool IntersectSphereCastVSModelEX(
		const Vector3& start,
		const Vector3& end,
		const float radius,
		const GLTFModel* model,
		const Matrix& model_world_transform,
		const TileData::CollideFlag flag,
		HitResult::ResultSphereCastVSModel& result);

}
