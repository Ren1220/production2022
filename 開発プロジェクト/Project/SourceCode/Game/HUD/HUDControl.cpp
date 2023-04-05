#include "HUDControl.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Game/General/GameSystem.h"
#include "Game/General/PlayerController.h"
#include "Game/General/Inventory.h"
#include "Game/General/Tutorial.h"
#include "Game/General/Pause.h"
#include "Game/General/GameText.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Actor/Actor.h"
#include "Game/Component/Item/Item.h"
#include "Game/Stage/StageManager.h"

/* --- 定義 ---- */

HUDControl::HUDControl()
	: timer(0.0f)
{
}

HUDControl::~HUDControl()
{
}

void HUDControl::Initialize(ID3D11Device* device)
{
	text_frame.Create(device, L"Data/Texture/HUD/Text/text_frame.png");
}

void HUDControl::Update(float elapsed_time)
{
	if (Tutorial::GetInstance().IsTutorial())
	{
		// 各操作方法のチュートリアルのフラグを立てる。
		UpdateTutorial(elapsed_time);
	}
}

void HUDControl::Render(ID3D11DeviceContext* dc)
{
	// ポーズ状態の時は描画しない
	if(PauseSystem::GetInstance().IsPause()) return;

	bool can_render_tutorial{ true };

	for (const auto& actor : GameSys.GetActorManager()->GetUpdateActors())
	{
		if (actor->CompareGroupName("Item"))
		{
			RenderItemText(dc, actor, can_render_tutorial);
		}
	}

	if (Tutorial::GetInstance().IsTutorial() && can_render_tutorial)
	{
		RenderTutorial(dc);
	}
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	auto& game_text{ GameSys.GetGameText() };

}

void HUDControl::UpdateTutorial(float elapsed_time)
{
	auto& tutorial{ Tutorial::GetInstance() };
	const auto& player{ GameSys.GetPlayerController()->GetPlayer() };

	timer -= elapsed_time;

	for (const auto& actor : GameSys.GetActorManager()->GetUpdateActors())
	{
		// 攻撃を与えるチュートリアルのフラグ
		if (tutorial.GetFlag(Tutorial::ShowAttackText) == Tutorial::NotDisplayed)
		{
			// 敵アクタ
			if (actor->CompareGroupName("Enemy"))
			{
				float dist{ player->GetWorldPosition().Distance(actor->GetWorldPosition()) };

				if (dist < 5.0f)
				{
					tutorial.SetFlag(Tutorial::ShowAttackText, Tutorial::Displaying);
				}
			}
		}

		
		if (actor->CompareGroupName("Item"))
		{
			// アイテムに近づいたら基本攻撃チュートリアルは表示されなくなる
			float dist{ actor->GetWorldPosition().Distance(player->GetWorldPosition()) };

			if (dist < 1.0f && 
				tutorial.GetFlag(Tutorial::ShowCounterText) != Tutorial::AlreadyDisplayed)
			{
				tutorial.SetFlag(Tutorial::ShowAttackText, Tutorial::AlreadyDisplayed);
				tutorial.SetFlag(Tutorial::ShowComboText, Tutorial::AlreadyDisplayed);
				tutorial.SetFlag(Tutorial::ShowDodgeText, Tutorial::AlreadyDisplayed);
				tutorial.SetFlag(Tutorial::ShowCounterText, Tutorial::AlreadyDisplayed);
			}
		}
	}

	// アイテム使用・エンチャントのチュートリアル
	if (auto item = GameSys.GetInventory()->GetSelectItem())
	{
		// アイテム使用のチュートリアルを優先する
		if (tutorial.GetFlag(Tutorial::ShowConsumeText) != Tutorial::AlreadyDisplayed)
		{
			// 使用可能なアイテムか
			if (item->GetComponent<Item>()->CanConsume(player))
			{
				// アイテム使用チュートリアルの表示
				tutorial.SetFlag(Tutorial::ShowConsumeText, Tutorial::Displaying);
			}
		}
		else if (tutorial.GetFlag(Tutorial::ShowEnchantText) != Tutorial::AlreadyDisplayed)
		{
			// エンチャントに使えるアイテムか
			if (item->GetComponent<Item>()->CanEnchant(player))
			{
				// エンチャントチュートリアルの表示
				tutorial.SetFlag(Tutorial::ShowEnchantText, Tutorial::Displaying);
			}
		}
	}

	// エンチャント完了時に追加のテキストを４秒間表示
	if (tutorial.GetFlag(Tutorial::ShowEnchantText) == Tutorial::AlreadyDisplayed &&
		tutorial.GetFlag(Tutorial::ShowEnchantText2) == Tutorial::NotDisplayed)
	{
		tutorial.SetFlag(Tutorial::ShowEnchantText2, Tutorial::Displaying);
		timer = 4.0f;
	}

	// ４秒経過したら表示終わり
	if (tutorial.GetFlag(Tutorial::ShowEnchantText2) == Tutorial::Displaying &&
		timer < 0.0f)
	{
		tutorial.SetFlag(Tutorial::ShowEnchantText2, Tutorial::AlreadyDisplayed);
	}
}

void HUDControl::RenderItemText(
	ID3D11DeviceContext* dc, 
	const std::shared_ptr<Actor>& actor,
	bool& can_render_tutorial)
{
	const auto& player{ GameSys.GetPlayerController()->GetPlayer() };

	float dist{ actor->GetWorldPosition().Distance(player->GetWorldPosition()) };

	if (dist < 1.0f)
	{
		if (GameSys.GetInventory()->IsInventorySpace())
		{
			DrawMessage(dc, L"Xでアイテムを取得");
			can_render_tutorial = false;
		}
		else
		{
			DrawMessage(dc, L"これ以上は所持できない");
			can_render_tutorial = false;
		}
	}
}

void HUDControl::RenderTutorial(ID3D11DeviceContext* dc)
{
	const auto& tutorial{ Tutorial::GetInstance() };

	if (tutorial.GetFlag(Tutorial::ShowAttackText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"RBで通常攻撃");
	}
	else if (tutorial.GetFlag(Tutorial::ShowComboText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"再びRBを入力して連続で攻撃が可能");
	}
	else if (tutorial.GetFlag(Tutorial::ShowSlashText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"RTで強攻撃");
	}
	else if (tutorial.GetFlag(Tutorial::ShowCounterText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"スローモーション中にRBでカウンター");
	}
	else if (tutorial.GetFlag(Tutorial::ShowDodgeText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"LBで回避　タイミングよく回避することでカウンター");
	}
	else if (tutorial.GetFlag(Tutorial::ShowConsumeText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"十字キー上でアイテムを使用");
	}
	else if (tutorial.GetFlag(Tutorial::ShowEnchantText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"十字キー下で武器にエンチャント");
	}
	else if (tutorial.GetFlag(Tutorial::ShowEnchantText2) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"各攻撃にエンチャント応じた追加効果　何度か使用するとエンチャント効果は消える");
	}
}

void HUDControl::DrawMessage(ID3D11DeviceContext* dc,const wchar_t* text)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	auto& game_text{ GameSys.GetGameText() };

	std::wstring_view wsv{ text };

	constexpr float font_size{ 20.0f };

	const float frame_width{ wsv.length() * 20.0f + 60.0f };

//	constexpr float text_offset_y

	// テキストフレームの描画
	texture_renderer->BeginDraw(dc);
	texture_renderer->Draw(dc,
		text_frame,
		{ 800.0f - (frame_width * 0.5f), 200.0f },
		{ frame_width, 100.0f },
		{ 0.0f, 0.0f },
		{ 256.0f, 256.0f },
		{ 1.0f, 1.0f, 1.0f, 0.5f },
		TexturePivot::LeftTop);
	texture_renderer->EndDraw(dc);
	
	// テキストの描画
	game_text->SetText(
		GameText::Layer::HUD,
		text,
		FontFamily::Kokumincho,
		20.0f,
		{ 800.0f - (frame_width * 0.5f), 200.0f },
		{ frame_width, 100.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}
