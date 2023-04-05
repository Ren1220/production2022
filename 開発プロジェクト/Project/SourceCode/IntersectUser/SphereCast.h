#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Intersect.h"

/* --- 前方宣言 ---- */

class GLTFModelData; 


/* 
	--- 概要 ---- 

	取得できる値 : 
		レイの発射点である、球体の中心から、球体が三角形に当たった時の球体の中心までの距離。
		球体と三角形の接点。

	
	※ レイキャストとは異なり、交点が始点からの直線状にあるとは限らない

	メリット：
		レイが当たらなくても球体が当たった場合に交点がとれる。
		三角形に平行なレイでもきっちり交点の算出ができる。
		連続したメッシュの隙間抜けが発生しない
		レイの交点とは別の三角形にめり込むやっかいなケースの処理も、半径を持ったレイを用いて、
　　　	   接点（緑点）ベースで算出されるため、めり込み処理が（基本的に）不要。
	
	デメリット：
		普通のレイキャストと比べて、処理が重い。
		ただし、４本レイと比較すると完全版でほぼ同等、EST版だと３割前後ぐらい高速。
		なお、４本レイ法は交差判定と交点算出は容易だが、接点の算出はかなり難しく、
		また接点算出の処理負荷やめり込み解消処理は、上記比較に含まれていない。

	EST : 高速誤差あり
*/


/* --- 宣言 ---- */

namespace IntersectUser
{

	/// <summary>
	/// 簡易版スフィアキャスト。詳細はHeaderにて。
	/// </summary>
	/// <param name="start">レイの始点</param>
	/// <param name="end">レイの終点</param>
	/// <param name="radius">スフィアキャストの半径</param>
	/// <param name="triangle_position">キャストするポリゴンの頂点</param>
	/// <param name="result">キャスト結果</param>
	/// <param name="is_front_conuter_clock_wise">ポリゴンの頂点の順番</param>
	/// <returns></returns>
	bool IntersectSphereCastVSTriangleEST(
		const Vector3& start,
		const Vector3& end,
		const float radius,
		const Vector3 triangle_position[3],
		HitResult::ResultRayVSTriangle& result);
}
