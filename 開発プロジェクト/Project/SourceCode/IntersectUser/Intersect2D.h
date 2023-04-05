#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */



/* --- �O���錾 ---- */



/* --- �錾 ---- */

namespace IntersectUser2D
{
	extern bool IntersectAABBVSCircle(
		float box_mx, float box_Mx,
		float box_my, float box_My,
		float sphere_pos_x,
		float sphere_pos_y,
		float sphere_radius);

	/// <summary>
	/// ���C��AABB�̌����B
	/// ����������true���čŋߓ_�܂ł̋������擾����B
	/// false�̏ꍇ�ł��A�������s�����Ă��邾���Ȃ�A�ŋߓ_�܂ł̋������擾�ł���B
	/// �����ł��Ȃ��ꍇ�����ɂ�-1������B
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
	/// ���C��AABB�̌����B
	/// ����������true���čŋߓ_�܂ł̋������擾����B
	/// false�̏ꍇ�ł��A�������s�����Ă��邾���Ȃ�A�ŋߓ_�܂ł̋������擾�ł���B
	/// �����ł��Ȃ��ꍇ�����ɂ�-1������B
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
