#include "TerrainGenerator.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/RandomFunction.h"
#include "Utility/ChronoTimer.h"

/* --- ��` ---- */

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

	// �n�`����
	MapGenerate(
		min_area_size, 
		min_room_size, 
		margin_between_area, 
		room_wall_size, 
		enemy_count, 
		item_count, 
		tr_data,
		out_terrain);

	// ���������n�`�����ɁA�G���e�B�e�B��z�u����
	MapPostProcess(enemy_count, item_count, tr_data, out_terrain);

	// �n�`�������̏��������Ă���
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
		L"���̍ŏ��T�C�Y�����̃p�����[�^�̉e���ŕs�����Ă���");


	uint32_t map_width { out_terrain.GetMapWidth() };
	uint32_t map_height{ out_terrain.GetMapHeight() };

	// ��ƂȂ���B����𕪊����āA���𑝂₷�B
	std::shared_ptr<TerrainData::Area> area_base{ 
		std::make_shared<TerrainData::Area>(0, 0, map_width - 1U, map_height - 1U) };
	out_tr_data.areas.emplace_back(area_base);

	// ��敪��
	SplitArea(area_base, min_area_size, out_tr_data);

	// �����ɕ��������
	MakeRoom(min_room_size, margin_between_area, room_wall_size, out_tr_data);

	// �������m���Ȃ��ʘH�����
	MakeWalkway(out_tr_data);

	// ���̏������ɃZ���̃X�e�[�g��ݒ肷��
	ModifyMap(out_tr_data, out_terrain);

	// �Z����300����i�߂�
	CellularAutomata(300U, out_terrain);

}

void TerrainGenerator::SplitArea(
	const std::shared_ptr<TerrainData::Area>& parent,
	uint32_t min_area_size,
	TerrainData& out_tr_data)
{
	// �c�A���̋��̑傫�����ŏ��l�ȉ��ɂȂ����ꍇ���̕������s���Ȃ��悤�ɂ���
	if (parent->Width() <= min_area_size * 2) { parent->can_split_h = false; }
	if (parent->Height() <= min_area_size * 2) { parent->can_split_v = false; }

	// �������s�\�ȏ��
	if (parent->can_split_h == false &&
		parent->can_split_v == false)
	{
		return;
	}

	// �e�G���A�̏����ƂɎq�G���A���쐬���A���X�g�ɒǉ�
	std::shared_ptr<TerrainData::Area> child{ 
		out_tr_data.areas.emplace_back(parent->CreateChild()) };

	// ���ɕ���
	if (parent->can_split_h)
	{
		uint32_t x; // �����ʒu
		// ���������ۂɁA�������ŏ��l�����ɂȂ�Ȃ��悤�ɂ���
		x = Random::GetValue(child->mx + min_area_size, child->Mx - min_area_size);

		// �����������A�������Đ������ꂽ��悪�E�ɂȂ�悤�ɂ���
		parent->Mx = x;
		child->mx = x;

		// ��x�������ꂽ�����͂��̕����ɕ�������Ȃ��悤�ɂ���
		parent->can_split_h = false;

		// �ʘH�̒ǉ�
		out_tr_data.walkways.emplace_back(
			std::make_shared<TerrainData::Walkway>(false, parent, child));

		// �������������ĕ�������
		SplitArea(parent, min_area_size, out_tr_data);
		SplitArea(child, min_area_size, out_tr_data);
	}	
	// �c�ɕ���
	if (parent->can_split_v)
	{
		uint32_t y; // �����ʒu
		// ���������ۂɁA�������ŏ��l�����ɂȂ�Ȃ��悤�ɂ���
		y = Random::GetValue(child->my +min_area_size, child->My - min_area_size );

		// �����������A�������Đ������ꂽ��悪�E�ɂȂ�悤�ɂ���
		parent->My = y;
		child->my = y;

		// ��x�������ꂽ�����͂��̕����ɕ�������Ȃ��悤�ɂ���
		parent->can_split_v = false;

		// �ʘH�̒ǉ�
		out_tr_data.walkways.emplace_back(
			std::make_shared<TerrainData::Walkway>(true, parent, child));

		// �������������ĕ�������
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

		// �ŏ� �����̍ŏ��l+�ǂ̕���
		// �ő� ���̑傫��-�����Ƌ��̊Ԃ̍ŏ��l
		uint32_t w{ Random::GetValue(
			min_room_size + room_wall_size * 2,
			area->Width() - margin_between_area * 2) };
		uint32_t h{ Random::GetValue(
			min_room_size + room_wall_size * 2,
			area->Height() - margin_between_area * 2) };

		// �ŏ� ���̈ʒu+�����Ƌ��̊Ԃ̍ŏ��l
		// �ő� ���̈ʒu-�����Ƌ��̊Ԃ̍ŏ��l-�������������̑傫��
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
		// �c���肵���G���A�̒ʘH
		if (walkway->is_v)
		{
			const auto& a0{ walkway->area[0] };
			const auto& a1{ walkway->area[1] };

			// �����̃v���O�����̎d�l��e(0)�͏㑤�ɂȂ�
			// �����̊p�ɒʘH�����Ȃ��悤�ɂ���
			uint32_t mx = Random::GetValue(a0->room.mx + 1, a0->room.Mx - 1);
			uint32_t my = a0->My; // �ʘH���m�̒��ԓ_�Ƃ��ċ���y���W���g��
			uint32_t Mx = Random::GetValue(a1->room.mx + 1, a1->room.Mx - 1);
			uint32_t My = a1->my; // �ʘH���m�̒��ԓ_�Ƃ��ċ���y���W���g��

			walkway->way[0] = { mx, a0->room.My - 1, mx, my + 1 };
			walkway->way[1] = { mx, my, Mx, My };
			walkway->way[2] = { Mx, My - 1, Mx, a1->room.my + 1 };
		}
		// �����肵���G���A�̒ʘH
		else
		{
			const auto& a0{ walkway->area[0] };
			const auto& a1{ walkway->area[1] };

			// �����̃v���O�����̎d�l��e(0)�͍����ɂȂ�
			uint32_t mx = a0->Mx; // �ʘH���m�̒��ԓ_�Ƃ��ċ���x���W���g��
			// �����̊p�ɒʘH�����Ȃ��悤�ɂ���
			uint32_t my = Random::GetValue(a0->room.my + 1, a0->room.My - 1);
			uint32_t Mx = a1->mx; // �ʘH���m�̒��ԓ_�Ƃ��ċ���x���W���g��
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
		// �����ɂ͐����Z����z�u
		{
			const uint32_t mx{ area->room.mx };
			const uint32_t Mx{ area->room.Mx };
			const uint32_t my{ area->room.my };
			const uint32_t My{ area->room.My };

			// ���������m�C�Y����邱�Ƃł��s�K���ȃZ���̓������N����
			for (uint32_t ci = mx, ri = my; ci <= Mx; ++ci) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = mx, ri = My; ci <= Mx; ++ci) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = mx, ri = my; ri <= My; ++ri) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
			for (uint32_t ci = Mx, ri = my; ri <= My; ++ri) { if (Random::GetValue(0, 100) > 5) { map[ci][ri].current_state = map[ci][ri].next_state = TerrainCell::Alive; } };
		}

		// ���ɂ͕s���Z����z�u
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

	// �����Ŏw�肵�����㕪�i�߂�
	for (uint32_t i = 0U; i < gen_count; ++i)
	{
		// �e�Z���̎��̐���̃X�e�[�g�����߂�B�i�X�e�[�g�̍X�V�͂܂��j
		CellNextGen(out_terrain);
		// ���̐���̃X�e�[�g�ɍX�V����B
		UpdateCellState(out_terrain);
	}

#ifdef _DEBUG
	t.Stop(true, L"�Z���̍X�V�ɂ����������v����");
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
			// ���̃X�e�[�g��ݒ�
			auto& next_state{ out_terrain.GetMap()[ci][ri].next_state };
			constexpr uint8_t filter{ TerrainCell::Alive | TerrainCell::Dead | TerrainCell::Obstacle };

			// ���݂̃Z�����������
			if (current_state & TerrainCell::Alive)
			{
				// �����ς݃t���O
				next_state |= TerrainCell::Traces;

				constexpr int min_population{ 2 };
				constexpr int max_population{ 3 };

				// �ő�l���������͍ŏ��l���𒴂���E�����Ǝ��g�����S��Ԃɂ���
				if (alive < min_population ||
					alive > max_population)
				{
					next_state.Set(TerrainCell::Dead, filter);
				}

				// ���Ǝ��R�Ȓn�`�ɂ��邽�߂̏���
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
			// ���݂̃Z�������S���
			else if (current_state & TerrainCell::Dead)
			{

				constexpr int reproduction{ 3 };
				// ����̃Z���̐���������v����ꍇ���g�𐶑���Ԃɂ���
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
	// ������A�Z���Ȃ��@�����@���S�@�s���@�Z������x�ł�������ԂɂȂ���
	int n{}, a{}, d{}, o{}, t{};

	// �C���f�b�N�X�̃Z���̏��
	auto count = [&](int64_t x, int64_t y) -> void
	{
		// �z��O�̓Z�������݂��Ȃ��i�A�N�Z�X�ᔽ�̖h�~���j
		if (x < 0LL || y < 0LL || 
			x >= static_cast<int64_t>(terrain.GetMapWidth()) ||
			y >= static_cast<int64_t>(terrain.GetMapHeight()))
		{
			++n;
			return;
		}

		const auto& state{ terrain.GetMap()[x][y].current_state.Get() };

		// �n�`�t���O����Z���X�V�Ɋ֌W���镔���𒊏o����
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
	// �G�A�A�C�e��
	SetEntity(enemy_count, item_count, tr_data, out_terrain);
	// �^�C���̃X�e�[�g�̌���(�Փ˔���ʁA���b�V���A�}�e���A��)
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
		// �G���A�̒��I
		size_t set_area_i{ Random::GetValue(0ULL, tr_data.areas.size() - 1ULL) };

		const auto& set_area{ tr_data.areas[set_area_i] };

		// �G�̔z�u�i�S�񒊑I�j
		for(int i = 0; i < 100; ++i)
		{
			uint32_t ri{ Random::GetValue(set_area->my, set_area->My - 1U) };
			uint32_t ci{ Random::GetValue(set_area->mx, set_area->Mx - 1U) };

			auto& tile{ map[ci][ri] };

			// �I�������C���f�b�N�X�����̏ꍇ�͓G��z�u�����I�I��
			if (tile.IsFloor())
			{
				// ���Ƀt���O�̗����Ă��Ȃ���
				if (tile.data.state == TileData::S_None)
				{
					tile.data.state = TileData::S_EnemySpawn;
					tile.data.entity = TileData::E_Random; // �o������G�̓����_��
					break;
				}
			}
		}
	}

	for (uint32_t ii = 0U; ii < item_count; ++ii)
	{
		// �G���A�̒��I
		size_t set_area_i{ Random::GetValue(0ULL, tr_data.areas.size() - 1ULL) };

		const auto& set_area{ tr_data.areas[set_area_i] };

		// �A�C�e���̔z�u�i�S�񒊑I�j
		for(int i = 0; i < 100; ++i)
		{
			uint32_t ri{ Random::GetValue(set_area->my, set_area->My - 1U) };
			uint32_t ci{ Random::GetValue(set_area->mx, set_area->Mx - 1U) };

			auto& tile{ map[ci][ri] };

			// �I�������C���f�b�N�X�����̏ꍇ�͓G��z�u�����I�I��
			if (tile.IsFloor())
			{
				// ���Ƀt���O�̗����Ă��Ȃ���
				if (tile.data.state == TileData::S_None)
				{
					tile.data.state = TileData::S_ItemSpawn;
					tile.data.entity = TileData::I_Random; // �o������A�C�e���̓����_��
					break;
				}
			}
		}
	}
}

void TerrainGenerator::SetNextFloor(Terrain& out_terrain)
{
	auto& map{ out_terrain.GetMap() };

	// �^�C���̔z�u�i�Ȃ��Ƃ�΂��̂Ŗ����j
	while (1)
	{
		uint32_t ci{ Random::GetValue(0U, out_terrain.GetMapWidth() - 1U) };
		uint32_t ri{ Random::GetValue(0U, out_terrain.GetMapHeight() - 1U) };

		auto& tile{ map[ci][ri] };

		// �I�������C���f�b�N�X�����̏ꍇ�͓G��z�u�����I�I��
		if (tile.IsFloor())
		{
			// ���Ƀt���O�̗����Ă��Ȃ���
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

	// �^�C���̔z�u�i�Ȃ��Ƃ�΂��̂Ŗ����j
	while (1)
	{
		uint32_t ci{ Random::GetValue(0U, out_terrain.GetMapWidth() - 1U) };
		uint32_t ri{ Random::GetValue(0U, out_terrain.GetMapHeight() - 1U) };

		auto& tile{ map[ci][ri] };

		// �I�������C���f�b�N�X�����̏ꍇ�͓G��z�u�����I�I��
		if (tile.IsFloor())
		{
			// ���Ƀt���O�̗����Ă��Ȃ���
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

	// ���̃^�C�������̎�
	if (tile.IsFloor())
	{
		tile.data.mesh = TileData::M_Floor;
	}
	// ���̃^�C�������łȂ�
	else
	{
		// XZ�����ł��̕ǂ�������ʂ�����
		if (ci <= 0LL || ri <= 0LL || ci >= width - 1LL || ri >= height - 1LL)
		{
			tile.data.mesh = TileData::M_Wall;
		}
		// XZ�����ł��̕ǂ�������ʂ�����
		else if (
			map[ci - 1LL][ri].IsFloor() ||
			map[ci + 1LL][ri].IsFloor() ||
			map[ci][ri - 1LL].IsFloor() ||
			map[ci][ri + 1LL].IsFloor())
		{
			tile.data.mesh = TileData::M_Wall;
		}
		// XZ�ǂ�������ʂ��Ȃ�
		else
		{
			tile.data.mesh = TileData::M_WallCeiling;
		}
	}
}

void TerrainGenerator::UpdateTileMaterial(TerrainTile& tile)
{
	// �e�^�C���̃��b�V���ƃX�e�[�g�ɂ���ă}�e���A�������肷��
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

	// �ǂ̃��b�V���̂�
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

			// ���̃^�C�������̎�
			if (tile.IsFloor())
			{
				tile.data.mesh = TileData::M_Floor;
			}
			// ���̃^�C�������łȂ�
			else
			{
				// XZ�����ł��̕ǂ�������ʂ�����
				if (ci < 0LL || ri < 0LL || ci >= width || ri >= height)
				{
					tile.data.mesh = TileData::M_Wall;
				}
				// XZ�����ł��̕ǂ�������ʂ�����
				else if (
					map[ci - 1LL][ri].IsFloor() ||
					map[ci + 1LL][ri].IsFloor() ||
					map[ci][ri - 1LL].IsFloor() ||
					map[ci][ri + 1LL].IsFloor())
				{
					tile.data.mesh = TileData::M_Wall;
				}
				// XZ�ǂ�������ʂ��Ȃ�
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

			// �e�^�C���̃��b�V���ƃX�e�[�g�ɂ���ă}�e���A�������肷��
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

			// �ǂ̃��b�V���̂�
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