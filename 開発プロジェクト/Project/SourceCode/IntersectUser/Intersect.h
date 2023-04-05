#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Arithmetic/Arithmetic.h"

/* --- 前方宣言 ---- */



/* --- 宣言 ---- */


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
	/// 任意の点　の　３頂点内における最近点　を求める
	/// </summary>
	/// <param name="near_triangle_poisiton0">頂点領域に任意の点が存在する頂点</param>
	/// <param name="near_triangle_poisiton1">頂点領域に任意の点が存在する頂点</param>
	/// <returns>三頂点の内側に任意の点が存在するか</returns>
	bool ClosestPositionPointTriangle(
		const Vector3& point,
		const Vector3 triangle_position[3],
		Vector3& near_poisiton,
		Vector3& near_triangle_poisiton0,
		Vector3& near_triangle_poisiton1);

	/// <summary>
	/// 任意の点　の　３頂点内における最近点　を求める
	/// </summary>
	/// <returns>三頂点の内側に任意の点が存在するか</returns>
	bool ClosestPositionPointTriangle(
		const Vector3& point,
		const Vector3 triangle_position[3],
		Vector3& near_poisiton);
	
	/// <summary>
	/// 球の中心点から最も近い頂点　を求め、球の中心点との距離　と　球の半径　を比較し、
	/// 交差しているか判定する。
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
	/// 球の中心点とレイの始点のベクトルをレイに射影して、
	/// 球の半径と射影して求めた距離を比較し、交差しているか判定する。
	/// </summary>
	bool IntersectRayVSSphere(
		const Vector3& start,
		const Vector3& end,
		const Vector3& sphere_position,
		const float radius,
		HitResult::ResultRayVSSphere& result);

	/// <summary>
	/// 球の中心点へのカプセルの最近点を求め、
	/// 最近点から球への距離と、各半径の和を比較する。
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
	/// カプセル同士の各線分上の最近点を求め、
	/// 最近点同士の距離と、各半径の和を比較する。
	/// </summary>
	bool IntersectCapsuleVSCapsule(
		const Vector3& a_start,
		const Vector3& a_end,
		const float a_rad,
		const Vector3& b_start,
		const Vector3& b_end,
		const float b_rad);

	/// <summary>
	/// カプセル同士の各線分上の最近点を求め、
	/// 最近点同士の距離と、各半径の和を比較する。
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
