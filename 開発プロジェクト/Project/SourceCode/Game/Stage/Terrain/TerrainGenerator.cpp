#include "TerrainGenerator.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Utility/RandomFunction.h"
#include "Utility/ChronoTimer.h"

/* --- 定義 ---- */

void TerrainGenerator::Generate(
	uint32_t map_width,
	uint32_t map_height,
	uint32_t min_area_size,
	uint32_t min_room_size,
	uint32_t margin_between_area,
	uint32_t room_wall_size,
	uint32_t enemy_count,
	uint32_t item_count,
	Terrain& out_terrain)
{
	out_terrain.Initialize(map_width, map_height);

	TerrainData tr_data{};

	// 地形生成
	MapGenerate(
		min_area_size, 
		min_room_size, 
		margin_between_area, 
		room_wall_size, 
		enemy_count, 
		item_count, 
		tr_data,
		out_terrain);

	// 生成した地形を元に、エンティティを配置する
	MapPostProcess(enemy_count, item_count, tr_data, out_terrain);

	// 地形生成時の情報を持っておく
	out_terrain.SetTerrainData(tr_data);
}

void TerrainGenerator::MapGenerate(
	uint32_t min_area_size,
	uint32_t min_room_size,
	uint32_t margin_between_area,
	uint32_t room_wall_size,
	uint32_t enemy_count,
	uint32_t item_count,
	TerrainData& out_tr_data,
	Terrain& out_terrain)
{
	_ASSERT_EXPR(
		min_area_size >= min_room_size + margin_between_area * 2 + room_wall_size * 2,
		L"区域の最小サイズが他のパラメータの影響で不足している");


	uint32_t map_width { out_terrain.GetMapWidth() };
	uint32_t map_height{ out_terrain.GetMapHeight() };

	// 基準となる区域。これを分割して、区域を増やす。
	std::shared_ptr<TerrainData::Area> area_base{ 
		std::make_shared<TerrainData::Area>(0, 0, map_width - 1U, map_height - 1U) };
	out_tr_data.areas.emplace_back(area_base);

	// 区域分割
	SplitArea(area_base, min_area_size, out_tr_data);

	// 区域内に部屋を作る
	MakeRoom(min_room_size, margin_between_area, room_wall_size, out_tr_data);

	// 部屋同士をつなぐ通路を作る
	MakeWalkway(out_tr_data);

	// 区域の情報を元にセルのステートを設定する
	ModifyMap(out_tr_data, out_terrain);

	// セルを300世代進める
	CellularAutomata(300U, out_terrain);

}

void TerrainGenerator::SplitArea(
	const std::shared_ptr<TerrainData::Area>& parent,
	uint32_t min_area_size,
	TerrainData& out_tr_data)
{
	// 縦、横の区域の大きさが最小値以下になった場合その分割を行えないようにする
	if (parent->Width() <= min_area_size * 2) { parent->can_split_h = false; }
	if (parent->Height() <= min_area_size * 2) { parent->can_split_v = false; }

	// 分割が不可能な状態
	if (parent->can_split_h == false &&
		parent->can_split_v == false)
	{
		return;
	}

	// 親エリアの情報もとに子エリアを作成し、リストに追加
	std::shared_ptr<TerrainData::Area> child{ 
		out_tr_data.areas.emplace_back(parent->CreateChild()) };

	// 横に分割
	if (parent->can_split_h)
	{
		uint32_t x; // 分割位置
		// 分割した際に、部屋が最小値未満にならないようにする
		x = Random::GetValue(child->mx + min_area_size, child->Mx - min_area_size);

		// 分割元が左、分割して生成された区域が右になるようにする
		parent->Mx = x;
		child->mx = x;

		// 一度分割された部屋はその方向に分割されないようにする
		parent->can_split_h = false;

		// 通路の追加
		out_tr_data.walkways.emplace_back(
			std::make_shared<TerrainData::Walkway>(false, parent, child));

		// 分割した区域を再分割する
		SplitArea(parent, min_area_size, out_tr_data);
		SplitArea(child, min_area_size, out_tr_data);
	}	
	// 縦に分割
	if (parent->can_split_v)
	{
		uint32_t y; // 分割位置
		// 分割した際に、部屋が最小値未満にならないようにする
		y = Random::GetValue(child->my +min_area_size, child->My - min_area_size );

		// 分割元が左、分割して生成された区域が右になるようにする
		parent->My = y;
		child->my = y;

		// 一度分割された部屋はその方向に分割されないようにする
		parent->can_split_v = false;

		// 通路の追加
		out_tr_data.walkways.emplace_back(
			std::make_shared<TerrainData::Walkway>(true, parent, child));

		// 分割した区域を再分割する
		SplitArea(parent, min_area_size, out_tr_data);
		SplitArea(child, min_area_size, out_tr_data);
	}
}

void TerrainGenerator::MakeRoom(
	uint32_t min_room_size,
	uint32_t margin_between_area,
	uint32_t room_wall_size,
	const TerrainData& tr_data)
{
	for (auto& area : tr_data.areas)
	{
		auto& room{ area->room };

		// 最小 部屋の最小値+壁の部分
		// 最大 区域の大きさ-部屋と区域の間の最小値
		uint32_t w{ Random::GetValue(
			min_room_size + room_wall_size * 2,
			area->Width() - margin_between_area * 2) };
		uint32_t h{ Random::GetValue(
			min_room_size + room_wall_size * 2,
			area->Height() - margin_between_area * 2) };

		// 最小 区域の位置+部屋と区域の間の最小値
		// 最大 区域の位置-部屋と区域の間の最小値-生成した部屋の大きさ
		uint32_t x{ Random::GetValue(
			area->mx + margin_between_area,
			area->Mx - margin_between_area - w
		) };
		uint32_t y{ Random::GetValue(
			area->my + margin_between_area,
			area->My - margin_between_area - h
		) };

		room.mx = x;
		room.my = y;
		room.Mx = x + w - 1U;
		room.My = y + h - 1U;
	}
}

void TerrainGenerator::MakeWalkway(
	const TerrainData& tr_data)
{
	for (auto& walkway : tr_data.walkways)
	{
		// 縦割りしたエリアの通路
		if (walkway->is_v)
		{
			const auto& a0{ walkway->area[0] };
			const auto& a1{ walkway->area[1] };

			// 分割のプログラムの仕様上親(0)は上側になる
			// 部屋の角に通路を作らないようにする
			uint32_t mx = Random::GetValue(a0->room.mx + 1, a0->room.Mx - 1);
			uint32_t my = a0->My; // 通路同士の中間点として区画のy座標を使う
			uint32_t Mx = Random::GetValue(a1->room.mx + 1, a1->room.Mx - 1);
			uint32_t My = a1->my; // 通路同士の中間点として区画のy座標を使う

			walkway->way[0] = { mx, a0->room.My - 1, mx, my + 1 };
			walkway->way[1] = { mx, my, Mx, My };
			walkway->way[2] = { Mx, My - 1, Mx, a1->room.my + 1 };
		}
		// 横割りしたエリアの通路
		else
		{
			const auto& a0{ walkway->area[0] };
			const auto& a1{ walkway->area[1] };

			// 分割のプログラムの仕様上親(0)は左側になる
			uint32_t mx = a0->Mx; // 通路同士の中間点として区画のx座標を使う
			// 部屋の角に通路を作らないようにする
			uint32_t my = Random::GetValue(a0->room.my + 1, a0->room.My - 1);
			uint32_t Mx = a1->mx; // 通路同士の中間点として区画のx座標を使う
			uint32_t My = Random::GetValue(a1->room.my + 1, a1->room.My - 1);

			walkway->way[0] = { a0->room.Mx - 1, my, mx + 1, my };
			walkway->way[1] = { mx, my, Mx, My };
			walkway->way[2] = { Mx - 1, My, a1->room.mx + 1, My };
		}
	}
}

void TerrainGenerator::ModifyMap(
	const TerrainData& tr_data,
	Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	auto modify_map_by_area = [&](TerrainCell::State state, const TerrainData::Rect* rect)
	{
		const uint32_t mx{ rect->mx };
		const uint32_t Mx{ rect->Mx };
		const uint32_t my{ rect->my };
		const uint32_t My{ rect->My };

		for (uint32_t ci = mx, ri = my; ci <= Mx; ++ci) { if(Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = state; map[ci][ri].next_state = state; } };
		for (uint32_t ci = mx, ri = My; ci <= Mx; ++ci) { if(Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = state; map[ci][ri].next_state = state; } };
		for (uint32_t ci = mx, ri = my; ri <= My; ++ri) { if(Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = state; map[ci][ri].next_state = state; } };
		for (uint32_t ci = Mx, ri = my; ri <= My; ++ri) { if(Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = state; map[ci][ri].next_state = state; } };
	};

	for (const auto& area : tr_data.areas)
	{
		// 部屋には生存セルを配置
		{
			const uint32_t mx{ area->room.mx };
			const uint32_t Mx{ area->room.Mx };
			const uint32_t my{ area->room.my };
			const uint32_t My{ area->room.My };

			// 少しだけノイズを作ることでより不規則なセルの動きを起こす
			for (uint32_t ci = mx, ri = my; ci <= Mx; ++ci) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = mx, ri = My; ci <= Mx; ++ci) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = mx, ri = my; ri <= My; ++ri) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = Mx, ri = my; ri <= My; ++ri) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
		}

		// 区域には不干渉セルを配置
		{
			const uint32_t mx{ area->mx };
			const uint32_t Mx{ area->Mx };
			const uint32_t my{ area->my };
			const uint32_t My{ area->My };

			for (uint32_t ci = mx, ri = my; ci <= Mx; ++ci) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Obstacle;};
			for (uint32_t ci = mx, ri = My; ci <= Mx; ++ci) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Obstacle;};
			for (uint32_t ci = mx, ri = my; ri <= My; ++ri) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Obstacle;};
			for (uint32_t ci = Mx, ri = my; ri <= My; ++ri) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Obstacle;};
		};
	}

	auto modify_map_by_walkway = [&](TerrainCell::State state, TerrainData::Rect& rect)
	{
		if (rect.mx > rect.Mx) std::swap(rect.mx, rect.Mx);
		if (rect.my > rect.My) std::swap(rect.my, rect.My);

		for (uint32_t ci = rect.mx, ri = rect.my; ci <= rect.Mx || ri <= rect.My; ++ci, ++ri)
		{
			ci = (std::min)(ci, rect.Mx);
			ri = (std::min)(ri, rect.My);

			map[ci][ri].current_state = state;
			map[ci][ri].next_state = state;
		}
	};

	for (const auto& walkway : tr_data.walkways)
	{

		auto& w0{ walkway->way[0] };
		auto& w1{ walkway->way[1] };
		auto& w2{ walkway->way[2] };


		modify_map_by_walkway(TerrainCell::Alive, w0);
		modify_map_by_walkway(TerrainCell::Alive, w1);
		modify_map_by_walkway(TerrainCell::Alive, w2);
	}
}

void TerrainGenerator::CellularAutomata(
	uint32_t gen_count, 
	Terrain& out_terrain)
{
#ifdef _DEBUG
	ChronoTimer t;
	t.Start();
#endif // _DEBUG

	// 引数で指定した世代分進める
	for (uint32_t i = 0U; i < gen_count; ++i)
	{
		// 各セルの次の世代のステートを決める。（ステートの更新はまだ）
		CellNextGen(out_terrain);
		// 次の世代のステートに更新する。
		UpdateCellState(out_terrain);
	}

#ifdef _DEBUG
	t.Stop(true, L"セルの更新にかかった合計時間");
#endif
}

void TerrainGenerator::CellNextGen(Terrain& out_terrain)
{
	for (uint32_t ri = 0U; ri < out_terrain.GetMapHeight(); ++ri)
	{
		for (uint32_t ci = 0U; ci < out_terrain.GetMapWidth(); ++ci)
		{
			const auto [none, alive, dead, obst, traces] { GetNeighborStateCount(out_terrain, ci, ri)};

			const auto& current_state{ out_terrain.GetMap()[ci][ri].current_state };
			// 次のステートを設定
			auto& next_state{ out_terrain.GetMap()[ci][ri].next_state };
			constexpr uint8_t filter{ TerrainCell::Alive | TerrainCell::Dead | TerrainCell::Obstacle };

			// 現在のセルが生存状態
			if (current_state & TerrainCell::Alive)
			{
				// 生存済みフラグ
				next_state |= TerrainCell::Traces;

				constexpr int min_population{ 2 };
				constexpr int max_population{ 3 };

				// 最大人口もしくは最小人口を超える・下回ると自身を死亡状態にする
				if (alive < min_population ||
					alive > max_population)
				{
					next_state.Set(TerrainCell::Dead, filter);
				}

				// 割と自然な地形にするための条件
				{
					if (dead >= 7)
					{
						if (Random::GetValue(0, 100) > 98)
						{
							next_state.Set(TerrainCell::Dead, filter);
						}
					}

					if (traces > 3)
					{
						if (Random::GetValue(0, 100) > 98)
						{
							next_state.Set(TerrainCell::Dead, filter);
						}
					}
				}
			}
			// 現在のセルが死亡状態
			else if (current_state & TerrainCell::Dead)
			{

				constexpr int reproduction{ 3 };
				// 周りのセルの生存数が一致する場合自身を生存状態にする
				if (alive == reproduction)
				{
					next_state.Set(TerrainCell::Alive, filter);
				}
			}
		}
	}
}

void TerrainGenerator::UpdateCellState(Terrain& out_terrain)
{
	for (uint32_t ri = 0U; ri < out_terrain.GetMapHeight(); ++ri)
	{
		for (uint32_t ci = 0U; ci < out_terrain.GetMapWidth(); ++ci)
		{
			auto& state{ out_terrain.GetMap()[ci][ri] };
			state.current_state = state.next_state;
		}
	}
}

std::tuple<int, int, int, int, int> TerrainGenerator::GetNeighborStateCount(
	const Terrain& terrain,
	uint32_t x, 
	uint32_t y)
{
	// 左から、セルなし　生存　死亡　不干渉　セルが一度でも生存状態になった
	int n{}, a{}, d{}, o{}, t{};

	// インデックスのセルの状態
	auto count = [&](int64_t x, int64_t y) -> void
	{
		// 配列外はセルが存在しない（アクセス違反の防止も）
		if (x < 0LL || y < 0LL || 
			x >= static_cast<int64_t>(terrain.GetMapWidth()) ||
			y >= static_cast<int64_t>(terrain.GetMapHeight()))
		{
			++n;
			return;
		}

		const auto& state{ terrain.GetMap()[x][y].current_state.Get() };

		// 地形フラグからセル更新に関係する部分を抽出する
		switch (state)
		{
		case TerrainCell::None:	      ++n; break;
		case TerrainCell::Alive:	  ++a; break;
		case TerrainCell::Dead:	      ++d; break;
		case TerrainCell::Obstacle:   ++o; break;

		case (TerrainCell::None     | TerrainCell::Traces): ++n; ++t; break;
		case (TerrainCell::Alive    | TerrainCell::Traces): ++a; ++t; break;
		case (TerrainCell::Dead     | TerrainCell::Traces): ++d; ++t; break;
		case (TerrainCell::Obstacle | TerrainCell::Traces): ++o; ++t; break;
		}
	};

	int64_t _x{ static_cast<uint32_t>(x) };
	int64_t _y{ static_cast<uint32_t>(y) };

	count(_x - 1LL, _y - 1LL);
	count(_x	  , _y - 1LL);
	count(_x + 1LL, _y - 1LL);

	count(_x - 1LL, _y);
	count(_x + 1LL, _y);

	count(_x - 1LL, _y + 1LL);
	count(_x      , _y + 1LL);
	count(_x + 1LL, _y + 1LL);

	return { n, a, d, o, t };

}

void TerrainGenerator::MapPostProcess(
	uint32_t enemy_count, 
	uint32_t item_count, 
	const TerrainData& tr_data,
	Terrain& out_terrain)
{
	SetPlayerSpawn(out_terrain);
	SetNextFloor(out_terrain);
	// 敵、アイテム
	SetEntity(enemy_count, item_count, tr_data, out_terrain);
	// タイルのステートの決定(衝突判定面、メッシュ、マテリアル)
	UpdateTile(out_terrain);
}

void TerrainGenerator::SetEntity(
	uint32_t enemy_count, 
	uint32_t item_count, 
	const TerrainData& tr_data,
	Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	for (uint32_t ei = 0U; ei < enemy_count; ++ei)
	{
		// エリアの抽選
		size_t set_area_i{ Random::GetValue(0ULL, tr_data.areas.size() - 1ULL) };

		const auto& set_area{ tr_data.areas[set_area_i] };

		// 敵の配置（百回抽選）
		for(int i = 0; i < 100; ++i)
		{
			uint32_t ri{ Random::GetValue(set_area->my, set_area->My - 1U) };
			uint32_t ci{ Random::GetValue(set_area->mx, set_area->Mx - 1U) };

			auto& tile{ map[ci][ri] };

			// 選択したインデックスが床の場合は敵を配置し抽選終了
			if (tile.IsFloor())
			{
				// 特にフラグの立っていない床
				if (tile.data.state == TileData::S_None)
				{
					tile.data.state = TileData::S_EnemySpawn;
					tile.data.entity = TileData::E_Random; // 出現する敵はランダム
					break;
				}
			}
		}
	}

	for (uint32_t ii = 0U; ii < item_count; ++ii)
	{
		// エリアの抽選
		size_t set_area_i{ Random::GetValue(0ULL, tr_data.areas.size() - 1ULL) };

		const auto& set_area{ tr_data.areas[set_area_i] };

		// アイテムの配置（百回抽選）
		for(int i = 0; i < 100; ++i)
		{
			uint32_t ri{ Random::GetValue(set_area->my, set_area->My - 1U) };
			uint32_t ci{ Random::GetValue(set_area->mx, set_area->Mx - 1U) };

			auto& tile{ map[ci][ri] };

			// 選択したインデックスが床の場合は敵を配置し抽選終了
			if (tile.IsFloor())
			{
				// 特にフラグの立っていない床
				if (tile.data.state == TileData::S_None)
				{
					tile.data.state = TileData::S_ItemSpawn;
					tile.data.entity = TileData::I_Random; // 出現するアイテムはランダム
					break;
				}
			}
		}
	}
}

void TerrainGenerator::SetNextFloor(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	// タイルの配置（ないとやばいので無限）
	while (1)
	{
		uint32_t ci{ Random::GetValue(0U, out_terrain.GetMapWidth() - 1U) };
		uint32_t ri{ Random::GetValue(0U, out_terrain.GetMapHeight() - 1U) };

		auto& tile{ map[ci][ri] };

		// 選択したインデックスが床の場合は敵を配置し抽選終了
		if (tile.IsFloor())
		{
			// 特にフラグの立っていない床
			if (tile.data.state == TileData::S_None)
			{
				tile.data.state = TileData::S_NextFloor;
				break;
			}
		}
	}
}

void TerrainGenerator::SetPlayerSpawn(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	// タイルの配置（ないとやばいので無限）
	while (1)
	{
		uint32_t ci{ Random::GetValue(0U, out_terrain.GetMapWidth() - 1U) };
		uint32_t ri{ Random::GetValue(0U, out_terrain.GetMapHeight() - 1U) };

		auto& tile{ map[ci][ri] };

		// 選択したインデックスが床の場合は敵を配置し抽選終了
		if (tile.IsFloor())
		{
			// 特にフラグの立っていない床
			if (tile.data.state == TileData::S_None)
			{
				tile.data.state = TileData::S_PlayerSpawn;
				break;
			}
		}
	}
}

void TerrainGenerator::UpdateTile(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	int64_t width{ static_cast<int64_t>(out_terrain.GetMapWidth()) };
	int64_t height{ static_cast<int64_t>(out_terrain.GetMapHeight()) };

	for (int64_t ri = 0LL; ri < height; ++ri)
	{
		for (int64_t ci = 0LL; ci < width; ++ci)
		{
			auto& tile{ map[ci][ri] };

			UpdateTileMesh(map, ci, ri, width, height);

			UpdateTileMaterial(tile);

			UpdateTileCollide(map, ci, ri, width, height);

		}
	}
}

void TerrainGenerator::UpdateTileMesh(
	std::vector<std::vector<TerrainTile>>& map,
	int64_t ci, int64_t ri,
	int64_t width, int64_t height)
{
	auto& tile{ map[ci][ri] };

	// このタイルが床の時
	if (tile.IsFloor())
	{
		tile.data.mesh = TileData::M_Floor;
	}
	// このタイルが床でない
	else
	{
		// XZ方向でこの壁が見える面がある
		if (ci <= 0LL || ri <= 0LL || ci >= width - 1LL || ri >= height - 1LL)
		{
			tile.data.mesh = TileData::M_Wall;
		}
		// XZ方向でこの壁が見える面がある
		else if (
			map[ci - 1LL][ri].IsFloor() ||
			map[ci + 1LL][ri].IsFloor() ||
			map[ci][ri - 1LL].IsFloor() ||
			map[ci][ri + 1LL].IsFloor())
		{
			tile.data.mesh = TileData::M_Wall;
		}
		// XZ壁が見える面がない
		else
		{
			tile.data.mesh = TileData::M_WallCeiling;
		}
	}
}

void TerrainGenerator::UpdateTileMaterial(TerrainTile& tile)
{
	// 各タイルのメッシュとステートによってマテリアルを決定する
	switch (tile.data.mesh)
	{
	case TileData::M_Floor:

		switch (tile.data.state)
		{
		case TileData::S_NextFloor:
			tile.material = TileMaterial::Floor::Warp;
			break;
		default:
			tile.material = TileMaterial::Floor::Default;
			break;
		}

		break;
	case TileData::M_Wall:
	case TileData::M_WallCeiling:

		tile.material = TileMaterial::Wall::Default;

		break;
	default:
		break;
	}
}

void TerrainGenerator::UpdateTileCollide(
	std::vector<std::vector<TerrainTile>>& map,
	int64_t ci, int64_t ri,
	int64_t width, int64_t height)
{
	auto& tile{ map[ci][ri] };

	// 壁のメッシュのみ
	if (tile.data.mesh != TileData::M_Wall) return;

	if (ci > 0)
	{
		if (map[ci - 1LL][ri].IsFloor())
		{
			tile.data.AddCollideFlag(TileData::C_CollideLeft | TileData::C_IsCollide);
		}
	}
	if (ci < width - 1LL)
	{
		if (map[ci + 1LL][ri].IsFloor())
		{
			tile.data.AddCollideFlag(TileData::C_CollideRight | TileData::C_IsCollide);
		}
	}
	if (ri > 0)
	{
		if (map[ci][ri - 1LL].IsFloor())
		{
			tile.data.AddCollideFlag(TileData::C_CollideDown | TileData::C_IsCollide);
		}
	}
	if (ri < height - 1LL)
	{
		if (map[ci][ri + 1LL].IsFloor())
		{
			tile.data.AddCollideFlag(TileData::C_CollideUp | TileData::C_IsCollide);
		}
	}
}

void TerrainGenerator::UpdateTileMesh(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	int64_t width{ static_cast<int64_t>(out_terrain.GetMapWidth()) };
	int64_t height{ static_cast<int64_t>(out_terrain.GetMapHeight()) };

	for (int64_t ri = 0LL; ri < height; ++ri)
	{
		for (int64_t ci = 0LL; ci < width; ++ci)
		{
			auto& tile{ map[ci][ri] };

			// このタイルが床の時
			if (tile.IsFloor())
			{
				tile.data.mesh = TileData::M_Floor;
			}
			// このタイルが床でない
			else
			{
				// XZ方向でこの壁が見える面がある
				if (ci < 0LL || ri < 0LL || ci >= width || ri >= height)
				{
					tile.data.mesh = TileData::M_Wall;
				}
				// XZ方向でこの壁が見える面がある
				else if (
					map[ci - 1LL][ri].IsFloor() ||
					map[ci + 1LL][ri].IsFloor() ||
					map[ci][ri - 1LL].IsFloor() ||
					map[ci][ri + 1LL].IsFloor())
				{
					tile.data.mesh = TileData::M_Wall;
				}
				// XZ壁が見える面がない
				else
				{
					tile.data.mesh = TileData::M_WallCeiling;
				}
			}
		}
	}
}

void TerrainGenerator::UpdateTileMaterial(Terrain& out_terrain)
{
	for (uint32_t ri = 0U; ri < out_terrain.GetMapHeight(); ++ri)
	{
		for (uint32_t ci = 0U; ci < out_terrain.GetMapWidth(); ++ci)
		{
			auto& tile{ out_terrain.GetMap()[ci][ri] };

			// 各タイルのメッシュとステートによってマテリアルを決定する
			switch (tile.data.mesh)
			{
			case TileData::M_Floor:

				switch (tile.data.state)
				{
				case TileData::S_NextFloor:
					tile.material = TileMaterial::Floor::Warp;
					break;
				default:
					tile.material = TileMaterial::Floor::Default;
					break;
				}

				break;
			case TileData::M_Wall:
			case TileData::M_WallCeiling:

				tile.material = TileMaterial::Wall::Default;

				break;
			default:
				break;
			}

		}
	}
}

void TerrainGenerator::UpdateTileCollide(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	int64_t width{ static_cast<int64_t>(out_terrain.GetMapWidth()) };
	int64_t height{ static_cast<int64_t>(out_terrain.GetMapHeight()) };

	for (int64_t ri = 0LL; ri < height; ++ri)
	{
		for (int64_t ci = 0LL; ci < width; ++ci)
		{
			auto& tile{ map[ci][ri] };

			// 壁のメッシュのみ
			if (tile.data.mesh != TileData::M_Wall) continue;

			if (ci > 0)
			{
				if (!map[ci - 1LL][ri].IsFloor())
				{
					tile.data.AddCollideFlag(TileData::C_CollideLeft | TileData::C_IsCollide);
				}
			}
			if (ci < width - 1LL)
			{
				if (!map[ci + 1LL][ri].IsFloor())
				{
					tile.data.AddCollideFlag(TileData::C_CollideRight | TileData::C_IsCollide);
				}
			}
			if (ri > 0)
			{
				if (!map[ci][ri - 1LL].IsFloor())
				{
					tile.data.AddCollideFlag(TileData::C_CollideUp | TileData::C_IsCollide);
				}
			}
			if (ri < height - 1LL)
			{
				if (!map[ci][ri + 1LL].IsFloor())
				{
					tile.data.AddCollideFlag(TileData::C_CollideDown | TileData::C_IsCollide);
				}
			}

		}
	}
}