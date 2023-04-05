#pragma once

/* --- 概要 ---- */

// ステージの基礎となる地形マップを生成するクラス。
// 区域分割法とセルオートマトン法を利用して生成する。
// 区域分割法にてマップを再帰的に分割して生成した区域の中に部屋として扱う部分を生成する。
// 次に部屋同士を区域を分割する境界部を用いてつなぐ。
// その後、セルオートマトンを利用して地形を自然な形にする。

/* --- ヘッダファイルのインクルード ---- */

#include <tuple>
#include <functional>

#include "Terrain.h"

/* --- 宣言 ---- */

class TerrainGenerator
{
public:

	/// <summary>
	/// 区域分割法を使って部屋、区域を生成しその生成した部分に、
	/// セルオートマトン法のステートを設定。これによって生成した部屋を自然な形に変え、
	/// それをマップとして出力する。
	/// </summary>
	/// <param name="map_width">生成するマップのサイズ</param>
	/// <param name="map_height">生成するマップのサイズ</param>
	/// <param name="min_area_size">分割する区域の最小サイズ(min_as >= min_rs + m_b_a * 2 + r_w_s * 2)でないとエラー</param>
	/// <param name="min_room_size">分割した区域内に生成する部屋の最小サイズ</param>
	/// <param name="margin_between_area">区域と部屋の空間のサイズ</param>
	/// <param name="room_wall_size">部屋の壁のサイズ</param>
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

private: // 区域分割法によるベースになる地形の生成


	/// <summary>
	/// 区域分割法を用いて、区域分割部分には不干渉のセル。
	/// 部屋の枠部分に生存セル。その他の部分に死亡せるを配置する。
	/// その後、セルオートマトン法で生成した部屋を自然な形にする。
	/// </summary>
	/// <param name="min_area_size">区域の最小サイズ。値が小さすぎると下つの値の影響でエラーになる。</param>
	/// <param name="min_room_size">部屋の最小サイズ</param>
	/// <param name="margin_between_area">区域と部屋の間の空間の最小サイズ</param>
	/// <param name="room_wall_size">部屋の壁のサイズ</param>
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
	/// 区域分割処理。引数parent縦又は横に分割し、それを指定した最小サイズなるまで再帰的に分割する。
	/// </summary>
	/// <param name="parent">分割する区域</param>
	/// <param name="min_area_size">区域の最小サイズ</param>
	static void SplitArea(
		const std::shared_ptr<TerrainData::Area>& parent,
		uint32_t min_area_size,
		TerrainData& out_tr_data);

	/// <summary>
	/// 区域分割処理で分割した区域内に実際の部屋を生成する。
	/// </summary>
	/// <param name="min_room_size">部屋の最小サイズ</param>
	/// <param name="margin_between_area"></param>
	/// <param name="room_wall_size">部屋の壁のサイズ</param>
	/// <param name="out_areas"></param>
	static void MakeRoom(
		uint32_t min_room_size,
		uint32_t margin_between_area,
		uint32_t room_wall_size,
		const TerrainData& tr_data);

	/// <summary>
	/// 区域分割処理で登録した部屋同士をつなぐ道を生成する。
	/// </summary>
	/// <param name="out_walkway"></param>
	static void MakeWalkway(
		const TerrainData& tr_data);

	/// <summary>
	/// 地形マップにステートを書き込む
	/// </summary>
	static void ModifyMap(
		const TerrainData& tr_data,
		Terrain& out_terrain);

private: // セルオートマトン法によって生成した地形をより洞窟のような感じにする

	/// <summary>
	/// 区域分割処理によって設定されたステートを元にセルオートマトン法によって地形を自然な形に削る。
	/// この処理の中で一度でも生存セルになった部分が床に、その他の部分が壁になる。
	/// </summary>
	/// <param name="gen_count">世代を多く進めることでより処理が進む、大体200回ぐらいで変化が少なくなる</param>
	/// <param name="out_terrain"></param>
	static void CellularAutomata(
		uint32_t gen_count, 
		Terrain& out_terrain);

	/// <summary>
	/// 各セルの周囲のセルの状態によって次の世代での自分のステートを決定する。
	/// </summary>
	/// <param name="out_terrain"></param>
	static void CellNextGen(Terrain& out_terrain);

	static void UpdateCellState(Terrain& out_terrain);

	/// <summary>
	/// 引数のインデックスのセルの周囲のセルの状態を取得し、各ステートの数を数える。
	/// </summary>
	/// <param name="terrain"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns>セルなし　生存　死亡　不干渉　セルが一度でも生存状態になった</returns>
	static std::tuple<int, int, int, int, int> GetNeighborStateCount(
		const Terrain& terrain,
		uint32_t x,
		uint32_t y);

private: // 後処理

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
	/// 各セルの状態からタイルデータ内のStateの値を決定する
	/// </summary>
	/// <param name="out_terrain"></param>
	static void UpdateTile(Terrain& out_terrain);

	/// <summary>
	/// タイルの状態からそのタイルに使用するメッシュ番号を設定する。
	/// </summary>
	static void UpdateTileMesh(
		std::vector<std::vector<TerrainTile>>& map,
		int64_t ci, int64_t ri,
		int64_t width, int64_t height);

	/// <summary>
	/// タイルの状態からそのタイルに使用するマテリアル番号を設定する。
	/// 先にメッシュを設定すること。
	/// </summary>
	static void UpdateTileMaterial(TerrainTile& tile);

	/// <summary>
	/// 壁メッシュを利用するタイルの衝突判定を行う面を設定する。
	/// 先にメッシュを設定すること。
	/// </summary>
	static void UpdateTileCollide(
		std::vector<std::vector<TerrainTile>>& map,
		int64_t ci, int64_t ri,
		int64_t width, int64_t height);

	/// <summary>
	/// タイルの状態からそのタイルに使用するメッシュ番号を設定する。
	/// </summary>
	static void UpdateTileMesh(Terrain& out_terrain);

	/// <summary>
	/// タイルの状態からそのタイルに使用するマテリアル番号を設定する。
	/// 先にメッシュを設定すること。
	/// </summary>
	static void UpdateTileMaterial(Terrain& out_terrain);

	/// <summary>
	/// 壁メッシュを利用するタイルの衝突判定を行う面を設定する。
	/// 先にメッシュを設定すること。
	/// </summary>
	static void UpdateTileCollide(Terrain& out_terrain);
};
