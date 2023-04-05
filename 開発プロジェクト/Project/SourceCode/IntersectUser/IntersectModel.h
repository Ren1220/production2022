#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Intersect.h"
#include "SphereCast.h"

/* --- 前方宣言 ---- */

class GLTFModel;

/* --- 宣言 ---- */

namespace IntersectUser
{
	namespace HitResult
	{
		struct ResultRayCastVSModel
		{
			Vector3 intersect_position;
			Vector3 triangle_normal;
			float distance;
			Vector3 triangle_vertices[3];
		};

		struct ResultSphereCastVSModel
		{
			Vector3 intersect_position;
			Vector3 triangle_normal;
			float distance;
			Vector3 triangle_vertices[3];
		};
	}

	bool IntersectRayCastVSModel(
		const Vector3& start,
		const Vector3& end,
		const GLTFModel* model,
		const Matrix& model_world_transform,
		HitResult::ResultRayCastVSModel& result);

	bool IntersectSphereCastVSModel(
		const Vector3& start,
		const Vector3& end,
		const float radius,
		const GLTFModel* model,
		HitResult::ResultSphereCastVSModel& result);

	bool IntersectSphereCastVSModel(
		const Vector3& start,
		const Vector3& end,
		const float radius,
		const GLTFModel* model,
		const Matrix& model_world_transform,
		HitResult::ResultSphereCastVSModel& result);

}
