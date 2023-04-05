#include "HUDManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Arithmetic/MyMath.h"
#include "Game/General/GameSystem.h"
#include "Game/General/GameText.h"
#include "Game/General/Title.h"
#include "Game/General/PlayerController.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Actor/Actor.h"
#include "General/AtaiEngine.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/Resource/Camera/Camera.h"


/* --- 定義 ---- */

void HUDManager::Initialize(ID3D11Device* device)
{
	hud_minimap.Initialize(device);
	hud_player.Initialize(device);
	hud_damage.Initialize();
	hud_control.Initialize(device);
	hud_health.Initialize(device);

	target_marker.Create(device);

	transparency = 0.0f;
	transparancy_delta = 0.0f;

	is_show_hud = false;
}

void HUDManager::Update(float elapsed_time)
{
	if (!TitleSystem::GetInstance().IsTitle())
	{
		transparancy_delta = 1.0f;
	}

	transparency = std::lerp(transparency, transparancy_delta, elapsed_time);

	hud_damage.Update(elapsed_time);
	hud_control.Update(elapsed_time);
	hud_health.Update(elapsed_time);
}

void HUDManager::Render(ID3D11DeviceContext* dc)
{
	if (is_show_hud)
	{
		auto camera = Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera();
		auto [width, height] { MySystem.GetScreenSize<float>() };

		DrawTargetMarker(dc, camera.get(), width, height);

		hud_health.Render(dc, camera.get(), width, height);
		hud_damage.Render(camera.get(), width, height);
		hud_minimap.Render(dc, transparency);
		hud_player.Render(dc, transparency);
		hud_control.Render(dc);
	}
}

void HUDManager::Reset()
{
	transparency = 0.0f;
	transparancy_delta = 0.0f;
	
	hud_health.Reset();
}

void HUDManager::DrawTargetMarker(
	ID3D11DeviceContext* dc, 
	const Camera* camera, 
	float screen_width, 
	float screen_height)
{
	if (auto target = GameSys.GetPlayerController()->GetTarget())
	{
		constexpr Vector3 world_offset{ 0.0f, 0.0f, 0.0f };
		const Vector3 world_pos{ target->GetWorldPosition() + world_offset };
		const Vector2 screen_pos{ Math::WorldToScreen(world_pos, camera->GetView(), camera->GetProjection(), screen_width, screen_height) };

		Graphics::GetInstance().GetTextureRenderer()->Draw(dc, target_marker, screen_pos, { 20.0f, 20.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, TexturePivot::Center);
	}
}
