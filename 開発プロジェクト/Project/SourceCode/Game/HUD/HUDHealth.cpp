#include "HUDHealth.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Arithmetic/MyMath.h"

#include "Game/Actor/ActorManager.h"
#include "Game/Actor/Actor.h"
#include "Game/Component/Health/Health.h"
#include "Game/General/GameSystem.h"
#include "Game/General/GameText.h"
#include "Game/General/PlayerController.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Camera/Camera.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- 定義 ---- */

void HUDHealth::Initialize(ID3D11Device* device)
{
	health_bar.Create(device);
	boss_health_bar.Create(device, L"Data/Texture/HUD/boss_health.png");
	boss_health_bar_frame.Create(device, L"Data/Texture/HUD/boss_health_frame.png");
	low_health_direction.Create(device, L"Data/Texture/ScreenPostEffect/damaged.png");
	show_boss_health = false;
	timer = 0.0f;
}

void HUDHealth::Reset()
{
	show_boss_health = false;
}

void HUDHealth::Update(float elapsed_time)
{
	timer += elapsed_time;
}

void HUDHealth::Render(ID3D11DeviceContext* dc, const Camera* camera, float screen_width, float screen_height)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	auto& game_text{ GameSys.GetGameText() };

	const Vector3& player_pos{ GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() };

	texture_renderer->BeginDraw(dc);

	// 敵の体力を表示する
	auto enemy_actors{ GameSys.GetActorManager()->FindActors({ "Enemy", "Boss" }) };
	for (const auto& actor : enemy_actors)
	{
		// 表示距離制限
		if (player_pos.Distance(actor->GetWorldPosition()) > 4.0f) continue;

		constexpr Vector3 world_offset{ 0.0f, -0.5f, 0.0f };
		const Vector3 world_pos{ actor->GetWorldPosition() + world_offset };
		const Vector2 screen_pos{ Math::WorldToScreen(world_pos, camera->GetView(), camera->GetProjection(), screen_width, screen_height) };

		constexpr Vector2 frame_size{ 100.0f, 5.0f };
		Vector2 frame_pos{ screen_pos.x - frame_size.x / 2, screen_pos.y - frame_size.y / 2 };

		auto health{ actor->GetComponent<Health>() };
		float ratio{ static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth()) };

		texture_renderer->Draw(dc, health_bar, frame_pos, { frame_size.x * ratio, frame_size.y }, { 0.75f, 0, 0, 1.0f }, TexturePivot::LeftTop);

		constexpr Vector2 text_offset{ 40.0f, 0.0f };

		if (health->GetHealth() > 0)
		{
			// テキストの描画
			game_text->SetText(
				GameText::Layer::HUD,
				std::to_wstring(health->GetHealth()).c_str(),
				FontFamily::SoukouMincho,
				15.0f,
				{ screen_pos.x + text_offset.x, screen_pos.y + text_offset.y },
				{ 100.0f , 10.0f },
				{ 1.0f, 1.0f, 1.0f, 0.8f });
		}
	}

	// ボスの体力
	if (show_boss_health)
	{
		// ボスグループのアクタを検索
		auto actors = GameSys.GetActorManager()->FindActors("Boss");

		if (!actors.empty())
		{
			const auto& actor{ actors.front() };

			auto health{ actor->GetComponent<Health>() };
			float ratio{ static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth()) };

			// 体力の描画
			texture_renderer->Draw(dc, boss_health_bar_frame, { 100.0f, screen_height - 70.0f }, { screen_width - 200.0f, 30.0f }, { 1, 1, 1, 1 }, TexturePivot::LeftTop);
			texture_renderer->Draw(dc, boss_health_bar, { 105.0f, screen_height - 65.0f }, { (screen_width - 210.0f) * ratio, 20.0f }, { 1, 0.8, 0, 1 }, TexturePivot::LeftTop);

			// テキストの描画
			game_text->SetText(
				GameText::Layer::HUD,
				std::to_wstring(health->GetHealth()).c_str(), FontFamily::Kokumincho, 30.0f, { 100.0f, screen_height - 30.0f }, { screen_width - 200.0f, 30.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING);
		}
	}

	// プレイヤー体力が少ないときに画面縁を赤くする
	{
		auto health{ GameSys.GetPlayerController()->GetPlayer()->GetComponent<Health>() };

		float ratio{ static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth()) };

		float opacity{ (sinf(timer * 3.14f) + 1.0f) };
		texture_renderer->Draw(dc, low_health_direction, { 0.0f, 0.0f }, { screen_width, screen_height }, { 1.0f, 1.0f, 1.0f, (0.8f - ratio * ratio) * opacity }, TexturePivot::LeftTop);
	}

	texture_renderer->EndDraw(dc);
}
