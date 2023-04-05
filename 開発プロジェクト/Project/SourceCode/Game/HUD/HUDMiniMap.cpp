#include "HUDMiniMap.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"

#include "Debug/DebugGUI.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Actor/Actor.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/Stage/StageManager.h"
#include "General/AtaiEngine.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

void HUDMiniMap::Initialize(ID3D11Device* device)
{
	minimap_frame.Create(device);
	terrain_tile.Create(device, L"Data/Texture/HUD/Minimap/terrain.png");
	entity_tile.Create(device, L"Data/Texture/HUD/Minimap/entity.png");
	//entity_tile.Create(device, L"Data/Texture/HUD/Inventory/entity_tile.png");
	
	minimap_cell_state.clear();
}

void HUDMiniMap::Reset()
{
	const auto& [width, height] { GameSys.GetStageManager()->GetStage().GetStageSize() };
	minimap_cell_state = std::vector(width, std::vector(height, None));
}

void HUDMiniMap::UpdateMimimap()
{
	const int64_t view_range{ 5 };

	const auto& [width, height] { GameSys.GetStageManager()->GetStage().GetStageSize() };

	if (minimap_cell_state.size() != static_cast<size_t>(width) ||
		minimap_cell_state.front().size() != static_cast<size_t>(height))
	{
		Reset();
	}

	auto player{ GameSys.GetPlayerController()->GetPlayer().get() };
	if (!player) return;

	// ���g�̂���ŋ߂̃Z��
	auto [c, r] { GameSys.GetStageManager()->GetNearestIndex(player) };

	// ���g�̂���Z���̎��͂̃X�e�[�g���X�V����B
	uint32_t cs{ static_cast<uint32_t>((std::max)(c - view_range, 0LL)) }, ce{ static_cast<uint32_t>((std::min)(c + view_range, static_cast<int64_t>(width))) };
	uint32_t rs{ static_cast<uint32_t>((std::max)(r - view_range, 0LL)) }, re{ static_cast<uint32_t>((std::min)(r + view_range, static_cast<int64_t>(height))) };

	for (uint32_t ri = 0U; ri < height; ++ri)
	{
		for (uint32_t ci = 0U; ci < width; ++ci)
		{
			uint8_t state{ minimap_cell_state[ci][ri] };
			if (ri >= rs && ri <= re && ci >= cs && ci <= ce)
			{
				state |= (CellState::Showing | CellState::Passed);
			}
			else
			{
				state &= ~CellState::Showing;
			}

			minimap_cell_state[ci][ri] = static_cast<CellState>(state);
		}
	}

}

void HUDMiniMap::Render(ID3D11DeviceContext* dc, float transparency)
{

	auto& m1{ DebugGUI::GetInstance().GetPerformanceGUI().StartMeasure("MapDraw") };

	UpdateMimimap();

	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };

	D3D11_VIEWPORT vp; UINT vn{ 1 };
	dc->RSGetViewports(&vn, &vp);

	constexpr float frame_margin_to_window{ 10.0f };
	const Vector2 frame_pos{ vp.Width - frame_size - frame_margin_to_window, frame_margin_to_window };

	texture_renderer->BeginDraw(dc, BSMode::Alpha);
	texture_renderer->Draw(dc, minimap_frame, frame_pos, { frame_size, frame_size }, { 0.1f,0.1f,0.1f,0.8f * transparency }, TexturePivot::LeftTop);
	texture_renderer->EndDraw(dc);

	float ts{ frame_size / static_cast<float>(map_size) };

	const auto& stage{ GameSys.GetStageManager()->GetStage() };
	const auto& [width, height] { stage.GetStageSize() };
	const auto& stage_manager{ GameSys.GetStageManager() };
	auto [px, py] { stage_manager->GetNearestIndex(GameSys.GetPlayerController()->GetPlayer().get())};

	// �v���C���[�̎��݂͂̂�`�悷��(�\������~�j�}�b�v�͈̔�)
	uint32_t cs{ static_cast<uint32_t>((std::max)(px - static_cast<uint32_t>(map_size / 2) + 1LL, 0LL)) }, ce{ static_cast<uint32_t>((std::min)(px + static_cast<uint32_t>(map_size/ 2), static_cast<int64_t>(width))) };
	// �\������~�j�}�b�v�͈̔�
	uint32_t rs{ static_cast<uint32_t>((std::max)(py - static_cast<uint32_t>(map_size / 2) + 1LL, 0LL)) }, re{ static_cast<uint32_t>((std::min)(py+ static_cast<uint32_t>(map_size/ 2), static_cast<int64_t>(height))) };

	auto calc_tile_pos = [&](uint32_t x, uint32_t y) -> Vector2
	{
		return { frame_pos.x + static_cast<float>((x - cs) * ts), (frame_pos.y +frame_size - ts) - static_cast<float>((y - rs) * ts) };
	};

	// �n�`�`��J�n
	texture_renderer->BeginDrawBatch(dc, BSMode::Alpha);

	for (uint32_t ri = rs; ri < re; ++ri)
	{
		for (uint32_t ci = cs; ci < ce; ++ci)
		{
			const auto& tile{ stage.GetTerrain().GetMap()[ci][ri] };

			Vector2 pos{ calc_tile_pos(ci, ri) };

			// �܂���������Ă��Ȃ�����
			if (minimap_cell_state[ci][ri] & None)
			{
				texture_renderer->DrawBatch(dc, terrain_tile, pos, { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 0.1f,0.1f,0.1f,transparency });
			}
			// �v���C���[�̎���
			else if (minimap_cell_state[ci][ri] & Showing)
			{
				// ��
				if (!tile.IsFloor())
				{
					texture_renderer->DrawBatch(dc, terrain_tile, pos, { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 1.0f,1.0f,1.0f,transparency });
				}
			}
			// �������ꂽ����
			else if (minimap_cell_state[ci][ri] & Passed)
			{
				// ��
				if (!tile.IsFloor())
				{
					texture_renderer->DrawBatch(dc, terrain_tile, pos, { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 1.0f,1.0f,1.0f,transparency });
				}

				// �v���C���[�̎��͂ł͂Ȃ����߁A�Â�����B
				texture_renderer->DrawBatch(dc, terrain_tile, pos, { ts, ts }, {0, 0}, { 256.0f, 256.0f }, { 0.0f, 0.0f, 0.0f, 0.8f * transparency });
			}
		}
	}

	// �n�`�`��I��
	texture_renderer->EndDrawBatch(dc, terrain_tile);

	// �G���e�B�e�B�`��J�n
	texture_renderer->BeginDrawBatch(dc);

	// �v���C���[
	texture_renderer->DrawBatch(dc, entity_tile, calc_tile_pos(static_cast<uint32_t>(px), static_cast<uint32_t>(py)), { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 0.0f,0.0f,1.0f,transparency });

	// �n�}�͈͓̔���
	auto is_map_range = [&](int64_t x, int64_t y) -> bool
	{
		uint32_t _x{ static_cast<uint32_t>(x) }, _y{ static_cast<uint32_t>(y) };

		return _x >= cs && _x < ce&& _y >= rs && _y < re;
	};

	auto is_view_range = [&](int64_t x, int64_t y) -> bool
	{
		uint32_t _x{ static_cast<uint32_t>(x) }, _y{ static_cast<uint32_t>(y) };

		return 	(minimap_cell_state[_x][_y] & Showing) > 0;
	};

	for (const auto& actor : GameSys.GetActorManager()->GetUpdateActors())
	{
		if (actor->CompareGroupName("Enemy"))
		{
			auto [x, y] { stage_manager->GetNearestIndex(actor.get())};
		
			if (is_map_range(x, y) && is_view_range(x, y))
			{
				// �v���C���[
				texture_renderer->DrawBatch(dc, entity_tile, calc_tile_pos(static_cast<uint32_t>(x), static_cast<uint32_t>(y)), { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 1.0f,0.0f,0.0f,transparency });
			}
		}
		else if (actor->CompareGroupName("Item"))
		{
			auto [x, y] { stage_manager->GetNearestIndex(actor.get())};
		
			if (is_map_range(x, y) && is_view_range(x, y))
			{
				// �v���C���[
				texture_renderer->DrawBatch(dc, entity_tile, calc_tile_pos(static_cast<uint32_t>(x), static_cast<uint32_t>(y)), { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 0.0f,1.0f,0.0f,transparency });
			}
		}
		else if (actor->CompareGroupName("FieldWeapon"))
		{
			auto [x, y] { stage_manager->GetNearestIndex(actor.get())};
		
			if (is_map_range(x, y) && is_view_range(x, y))
			{
				// �v���C���[
				texture_renderer->DrawBatch(dc, entity_tile, calc_tile_pos(static_cast<uint32_t>(x), static_cast<uint32_t>(y)), { ts, ts }, { 0, 0 }, { 256.0f, 256.0f }, { 0.0f,1.0f,0.0f,transparency });
			}
		}
	}

	texture_renderer->EndDrawBatch(dc, entity_tile);

	m1.StopMeasure();
}

void HUDMiniMap::ImGuiRender()
{
	ImGui::SliderFloat("FrameSize", &frame_size, 100.0f, 500.0f);
	int m{ static_cast<int>(map_size) };
	ImGui::SliderInt("MapSize", &m, 1, static_cast<int>(frame_size));
	map_size = static_cast<uint32_t>(m);

}
