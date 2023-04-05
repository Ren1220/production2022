#pragma once

/* --- �T�v ---- */

// �X�e�[�W�̊�b�ƂȂ�n�`�}�b�v�𐶐�����N���X�B
// ��敪���@�ƃZ���I�[�g�}�g���@�𗘗p���Đ�������B
// ��敪���@�ɂă}�b�v���ċA�I�ɕ������Đ����������̒��ɕ����Ƃ��Ĉ��������𐶐�����B
// ���ɕ������m�����𕪊����鋫�E����p���ĂȂ��B
// ���̌�A�Z���I�[�g�}�g���𗘗p���Ēn�`�����R�Ȍ`�ɂ���B

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <tuple>
#include <functional>

#include "Terrain.h"

/* --- �錾 ---- */

class TerrainGenerator
{
public:

	/// <summary>
	/// ��敪���@���g���ĕ����A���𐶐������̐������������ɁA
	/// �Z���I�[�g�}�g���@�̃X�e�[�g��ݒ�B����ɂ���Đ����������������R�Ȍ`�ɕς��A
	/// ������}�b�v�Ƃ��ďo�͂���B
	/// </summary>
	/// <param name="map_width">��������}�b�v�̃T�C�Y</param>
	/// <param name="map_height">��������}�b�v�̃T�C�Y</param>
	/// <param name="min_area_size">����������̍ŏ��T�C�Y(min_as >= min_rs + m_b_a * 2 + r_w_s * 2)�łȂ��ƃG���[</param>
	/// <param name="min_room_size">�������������ɐ������镔���̍ŏ��T�C�Y</param>
	/// <param name="margin_between_area">���ƕ����̋�Ԃ̃T�C�Y</param>
	/// <param name="room_wall_size">�����̕ǂ̃T�C�Y</param>
	/// <param name="out_terrain"></param>
	static void Generate(
		uint32_t map_width,
		uint32_t map_height,
		uint32_t min_area_size,
		uint32_t min_room_size,
		uint32_t margin_between_area,
		uint32_t room_wall_size,
		uint32_t enemy_count,
		uint32_t item_count,
		Terrain& out_terrain);

private: // ��敪���@�ɂ��x�[�X�ɂȂ�n�`�̐���


	/// <summary>
	/// ��敪���@��p���āA��敪�������ɂ͕s���̃Z���B
	/// �����̘g�����ɐ����Z���B���̑��̕����Ɏ��S�����z�u����B
	/// ���̌�A�Z���I�[�g�}�g���@�Ő����������������R�Ȍ`�ɂ���B
	/// </summary>
	/// <param name="min_area_size">���̍ŏ��T�C�Y�B�l������������Ɖ��̒l�̉e���ŃG���[�ɂȂ�B</param>
	/// <param name="min_room_size">�����̍ŏ��T�C�Y</param>
	/// <param name="margin_between_area">���ƕ����̊Ԃ̋�Ԃ̍ŏ��T�C�Y</param>
	/// <param name="room_wall_size">�����̕ǂ̃T�C�Y</param>
	/// <param name="out_terrain"></param>
	static void MapGenerate(
		uint32_t min_area_size,
		uint32_t min_room_size,
		uint32_t margin_between_area,
		uint32_t room_wall_size,
		uint32_t enemy_count,
		uint32_t item_count,
		TerrainData& out_tr_data,
		Terrain& out_terrain);

	/// <summary>
	/// ��敪�������B����parent�c���͉��ɕ������A������w�肵���ŏ��T�C�Y�Ȃ�܂ōċA�I�ɕ�������B
	/// </summary>
	/// <param name="parent">����������</param>
	/// <param name="min_area_size">���̍ŏ��T�C�Y</param>
	static void SplitArea(
		const std::shared_ptr<TerrainData::Area>& parent,
		uint32_t min_area_size,
		TerrainData& out_tr_data);

	/// <summary>
	/// ��敪�������ŕ������������Ɏ��ۂ̕����𐶐�����B
	/// </summary>
	/// <param name="min_room_size">�����̍ŏ��T�C�Y</param>
	/// <param name="margin_between_area"></param>
	/// <param name="room_wall_size">�����̕ǂ̃T�C�Y</param>
	/// <param name="out_areas"></param>
	static void MakeRoom(
		uint32_t min_room_size,
		uint32_t margin_between_area,
		uint32_t room_wall_size,
		const TerrainData& tr_data);

	/// <summary>
	/// ��敪�������œo�^�����������m���Ȃ����𐶐�����B
	/// </summary>
	/// <param name="out_walkway"></param>
	static void MakeWalkway(
		const TerrainData& tr_data);

	/// <summary>
	/// �n�`�}�b�v�ɃX�e�[�g����������
	/// </summary>
	static void ModifyMap(
		const TerrainData& tr_data,
		Terrain& out_terrain);

private: // �Z���I�[�g�}�g���@�ɂ���Đ��������n�`����蓴�A�̂悤�Ȋ����ɂ���

	/// <summary>
	/// ��敪�������ɂ���Đݒ肳�ꂽ�X�e�[�g�����ɃZ���I�[�g�}�g���@�ɂ���Ēn�`�����R�Ȍ`�ɍ��B
	/// ���̏����̒��ň�x�ł������Z���ɂȂ������������ɁA���̑��̕������ǂɂȂ�B
	/// </summary>
	/// <param name="gen_count">����𑽂��i�߂邱�Ƃł�菈�����i�ށA���200�񂮂炢�ŕω������Ȃ��Ȃ�</param>
	/// <param name="out_terrain"></param>
	static void CellularAutomata(
		uint32_t gen_count, 
		Terrain& out_terrain);

	/// <summary>
	/// �e�Z���̎��͂̃Z���̏�Ԃɂ���Ď��̐���ł̎����̃X�e�[�g�����肷��B
	/// </summary>
	/// <param name="out_terrain"></param>
	static void CellNextGen(Terrain& out_terrain);

	static void UpdateCellState(Terrain& out_terrain);

	/// <summary>
	/// �����̃C���f�b�N�X�̃Z���̎��͂̃Z���̏�Ԃ��擾���A�e�X�e�[�g�̐��𐔂���B
	/// </summary>
	/// <param name="terrain"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns>�Z���Ȃ��@�����@���S�@�s���@�Z������x�ł�������ԂɂȂ���</returns>
	static std::tuple<int, int, int, int, int> GetNeighborStateCount(
		const Terrain& terrain,
		uint32_t x,
		uint32_t y);

private: // �㏈��

	static void MapPostProcess(
		uint32_t enemy_count,
		uint32_t item_count,
		const TerrainData& tr_data,
		Terrain& out_terrain);

	static void SetEntity(
		uint32_t enemy_count,
		uint32_t item_count,
		const TerrainData& tr_data,
		Terrain& out_terrain);

	static void SetNextFloor(Terrain& out_terrain);

	static void SetPlayerSpawn(Terrain& out_terrain);

public:

	/// <summary>
	/// �e�Z���̏�Ԃ���^�C���f�[�^����State�̒l�����肷��
	/// </summary>
	/// <param name="out_terrain"></param>
	static void UpdateTile(Terrain& out_terrain);

	/// <summary>
	/// �^�C���̏�Ԃ��炻�̃^�C���Ɏg�p���郁�b�V���ԍ���ݒ肷��B
	/// </summary>
	static void UpdateTileMesh(
		std::vector<std::vector<TerrainTile>>& map,
		int64_t ci, int64_t ri,
		int64_t width, int64_t height);

	/// <summary>
	/// �^�C���̏�Ԃ��炻�̃^�C���Ɏg�p����}�e���A���ԍ���ݒ肷��B
	/// ��Ƀ��b�V����ݒ肷�邱�ƁB
	/// </summary>
	static void UpdateTileMaterial(TerrainTile& tile);

	/// <summary>
	/// �ǃ��b�V���𗘗p����^�C���̏Փ˔�����s���ʂ�ݒ肷��B
	/// ��Ƀ��b�V����ݒ肷�邱�ƁB
	/// </summary>
	static void UpdateTileCollide(
		std::vector<std::vector<TerrainTile>>& map,
		int64_t ci, int64_t ri,
		int64_t width, int64_t height);

	/// <summary>
	/// �^�C���̏�Ԃ��炻�̃^�C���Ɏg�p���郁�b�V���ԍ���ݒ肷��B
	/// </summary>
	static void UpdateTileMesh(Terrain& out_terrain);

	/// <summary>
	/// �^�C���̏�Ԃ��炻�̃^�C���Ɏg�p����}�e���A���ԍ���ݒ肷��B
	/// ��Ƀ��b�V����ݒ肷�邱�ƁB
	/// </summary>
	static void UpdateTileMaterial(Terrain& out_terrain);

	/// <summary>
	/// �ǃ��b�V���𗘗p����^�C���̏Փ˔�����s���ʂ�ݒ肷��B
	/// ��Ƀ��b�V����ݒ肷�邱�ƁB
	/// </summary>
	static void UpdateTileCollide(Terrain& out_terrain);
};
