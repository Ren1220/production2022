#pragma once

/* --- ヘッダファイルのインクルード ---- */



/* --- 前方宣言 ---- */



/* --- 宣言 ---- */

namespace IntersectUser2D
{
	extern bool IntersectAABBVSCircle(
		float box_mx, float box_Mx,
		float box_my, float box_My,
		float sphere_pos_x,
		float sphere_pos_y,
		float sphere_radius);

	/// <summary>
	/// レイとAABBの交差。
	/// 交差したらtrueして最近点までの距離を取得する。
	/// falseの場合でも、距離が不足しているだけなら、最近点までの距離を取得できる。
	/// 交差できない場合距離には-1が入る。
	/// </summary>
	/// <returns></returns>
	extern bool IntersectRayVSAABB(
		float ray_pos_x, float ray_pos_y,
		float ray_dir_x, float ray_dir_y,
		float ray_length,
		float box_pos_x, float box_pos_y,
		float box_width, float box_height,
		float& out_near_len,
		float& out_norm_x, float& out_norm_y);

	/// <summary>
	/// レイとAABBの交差。
	/// 交差したらtrueして最近点までの距離を取得する。
	/// falseの場合でも、距離が不足しているだけなら、最近点までの距離を取得できる。
	/// 交差できない場合距離には-1が入る。
	/// </summary>
	/// <returns></returns>
	extern bool IntersectRayVSAABB(
		float ray_pos_x, float ray_pos_y,
		float ray_dir_x, float ray_dir_y,
		float ray_length,
		float box_pos_x, float box_pos_y,
		float box_width, float box_height,
		float& out_near_len);
}
