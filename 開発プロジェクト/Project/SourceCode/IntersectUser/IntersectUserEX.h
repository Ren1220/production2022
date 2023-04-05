#pragma once

// このゲーム用のあまり汎用的でないもの

/* --- ヘッダファイルのインクルード ---- */

#include "IntersectModel.h"
#include "Game/Stage/Terrain/TileData.h"

/* --- 前方宣言 ---- */



/* --- 宣言 ---- */


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
