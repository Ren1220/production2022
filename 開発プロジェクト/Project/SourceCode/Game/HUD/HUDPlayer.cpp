#include "HUDPlayer.h"

/* --- 追加のヘッダファイルのインクルード ---- */


#include "Game/Actor/Actor.h"
#include "Game/Component/Item/Item.h"
#include "Game/General/GameSystem.h"
#include "Game/General/Inventory.h"
#include "Game/General/PlayerController.h"
#include "Game/Component/Weapon/WeaponHandler.h"
#include "Game/Component/Health/Health.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/Text/TextRenderer.h"

/* --- 定義 ---- */


HUDPlayer::HUDPlayer()
	: inventory_frame(), weapon_icon(), health_bar()
{
}

HUDPlayer::~HUDPlayer()
{
}

void HUDPlayer::Initialize(ID3D11Device* device)
{
	inventory_frame.Create(device, L"Data/Texture/HUD/Inventory/inventory_frame.png");
	weapon_icon.Create(device, L"Data/Texture/HUD/Inventory/Weapon/sword.png");
	health_bar.Create(device, L"Data/Texture/HUD/health_bar.png");
	health_bar_frame.Create(device, L"Data/Texture/HUD/health_bar_frame.png");
}

void HUDPlayer::Render(ID3D11DeviceContext* dc, float transparency)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	
	D3D11_VIEWPORT vp; UINT vn{ 1 };
	dc->RSGetViewports(&vn, &vp);

	texture_renderer->BeginDraw(dc, BSMode::Alpha);
	RenderHealth(dc, vp.Width, vp.Height, transparency);
	RenderInventory(dc, vp.Width, vp.Height, transparency);
	texture_renderer->EndDraw(dc);
}

void HUDPlayer::RenderHealth(ID3D11DeviceContext* dc, float screen_width, float screen_height, float transparency)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };

	if (auto player = GameSys.GetPlayerController()->GetPlayer())
	{
		auto health{ player->GetComponent<Health>() };
		
		// プレイヤーの体力割合の計算
		float ratio{ static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth()) };

		// 描画
		//texture_renderer.BeginDraw(dc);
		texture_renderer->Draw(dc, health_bar, { bar_offset.x, bar_offset.y }, { bar_size.x * ratio, bar_size.y }, { 1.0f, 1.0f, 1.0f, transparency }, TexturePivot::LeftTop);
		texture_renderer->Draw(dc, health_bar_frame, { bar_offset.x, bar_offset.y }, { bar_size.x, bar_size.y }, { 1.0f, 1.0f, 1.0f, transparency }, TexturePivot::LeftTop);
		//texture_renderer.EndDraw(dc);
	}
}

void HUDPlayer::RenderInventory(ID3D11DeviceContext* dc, float screen_width, float screen_height, float transparency)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	auto& text_renderer{ Graphics::GetInstance().GetTextRenderer() };

	const float y{ frame_offset.y + frame_margin_to_bar };


	// インベントリのフレームの描画
	texture_renderer->Draw(dc, inventory_frame, { frame_offset.x, y }, select_frame_size, { 1.0f, 1.0f, 1.0f, transparency }, TexturePivot::LeftTop);

	// インベントリの選択中のアイテムを表示する
	if (auto item = GameSys.GetInventory()->GetSelectItem())
	{
		const auto& data{ item->GetComponent<Item>()->GetItemData() };

		texture_renderer->Draw(
			dc, data.image, 
			{ frame_offset.x, y }, 
			select_frame_size, { data.color.x, data.color.y, data.color.z, data.color.w * transparency }, TexturePivot::LeftTop);

		text_renderer->SetText(
			data.name.c_str(),
			FontFamily::Kokumincho, 
			20.0f, 
			{ frame_offset.x, y + select_frame_size.y },
			{ select_frame_size.x, 20.0f },
			{ 1.0f, 1.0f, 1.0f, transparency },
			DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	const auto& inventory{ GameSys.GetInventory() };

	// 選択していないアイテム
	for (int i = 0; i < static_cast<int>(Inventory::GetInventorySize() - 1ULL); ++i)
	{
		texture_renderer->Draw(dc, inventory_frame,
			{ frame_offset.x  + select_frame_size.x + frame_margin + (frame_size.x + frame_margin) * i, y}, 
			frame_size, {1.0f, 1.0f, 1.0f, transparency }, TexturePivot::LeftTop);

		if (auto item = inventory->GetItem((inventory->GetSelectItemIndex() + (i + 1)) % 5))
		{
			const auto& data{ item->GetComponent<Item>()->GetItemData() };

			texture_renderer->Draw(
				dc, data.image,
				{ frame_offset.x + select_frame_size.x + frame_margin + (frame_size.x + frame_margin) * i, y },
				frame_size, { 1.0f, 1.0f, 1.0f, transparency }, TexturePivot::LeftTop);
		}
	}

	//// 現在所持している武器を表示する
	//if (auto player = GameSys.GetPlayerController()->GetPlayer())
	//{
	//	auto weapon_handler{ player->GetComponent<WeaponHandler>() };

	//	if (auto weapon = weapon_handler->GetWeaponActor())
	//	{
	//		texture_renderer.Draw(dc, weapon_icon, { frame_offset.x, y }, frame_size, { 1.0f, 1.0f, 1.0f, 1.0f }, TexturePivot::LeftTop);
	//	}
	//}
}
