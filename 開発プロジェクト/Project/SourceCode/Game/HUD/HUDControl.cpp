#include "HUDControl.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

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

/* --- ��` ---- */

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
		// �e������@�̃`���[�g���A���̃t���O�𗧂Ă�B
		UpdateTutorial(elapsed_time);
	}
}

void HUDControl::Render(ID3D11DeviceContext* dc)
{
	// �|�[�Y��Ԃ̎��͕`�悵�Ȃ�
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
		// �U����^����`���[�g���A���̃t���O
		if (tutorial.GetFlag(Tutorial::ShowAttackText) == Tutorial::NotDisplayed)
		{
			// �G�A�N�^
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
			// �A�C�e���ɋ߂Â������{�U���`���[�g���A���͕\������Ȃ��Ȃ�
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

	// �A�C�e���g�p�E�G���`�����g�̃`���[�g���A��
	if (auto item = GameSys.GetInventory()->GetSelectItem())
	{
		// �A�C�e���g�p�̃`���[�g���A����D�悷��
		if (tutorial.GetFlag(Tutorial::ShowConsumeText) != Tutorial::AlreadyDisplayed)
		{
			// �g�p�\�ȃA�C�e����
			if (item->GetComponent<Item>()->CanConsume(player))
			{
				// �A�C�e���g�p�`���[�g���A���̕\��
				tutorial.SetFlag(Tutorial::ShowConsumeText, Tutorial::Displaying);
			}
		}
		else if (tutorial.GetFlag(Tutorial::ShowEnchantText) != Tutorial::AlreadyDisplayed)
		{
			// �G���`�����g�Ɏg����A�C�e����
			if (item->GetComponent<Item>()->CanEnchant(player))
			{
				// �G���`�����g�`���[�g���A���̕\��
				tutorial.SetFlag(Tutorial::ShowEnchantText, Tutorial::Displaying);
			}
		}
	}

	// �G���`�����g�������ɒǉ��̃e�L�X�g���S�b�ԕ\��
	if (tutorial.GetFlag(Tutorial::ShowEnchantText) == Tutorial::AlreadyDisplayed &&
		tutorial.GetFlag(Tutorial::ShowEnchantText2) == Tutorial::NotDisplayed)
	{
		tutorial.SetFlag(Tutorial::ShowEnchantText2, Tutorial::Displaying);
		timer = 4.0f;
	}

	// �S�b�o�߂�����\���I���
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
			DrawMessage(dc, L"X�ŃA�C�e�����擾");
			can_render_tutorial = false;
		}
		else
		{
			DrawMessage(dc, L"����ȏ�͏����ł��Ȃ�");
			can_render_tutorial = false;
		}
	}
}

void HUDControl::RenderTutorial(ID3D11DeviceContext* dc)
{
	const auto& tutorial{ Tutorial::GetInstance() };

	if (tutorial.GetFlag(Tutorial::ShowAttackText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"RB�Œʏ�U��");
	}
	else if (tutorial.GetFlag(Tutorial::ShowComboText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"�Ă�RB����͂��ĘA���ōU�����\");
	}
	else if (tutorial.GetFlag(Tutorial::ShowSlashText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"RT�ŋ��U��");
	}
	else if (tutorial.GetFlag(Tutorial::ShowCounterText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"�X���[���[�V��������RB�ŃJ�E���^�[");
	}
	else if (tutorial.GetFlag(Tutorial::ShowDodgeText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"LB�ŉ���@�^�C�~���O�悭������邱�ƂŃJ�E���^�[");
	}
	else if (tutorial.GetFlag(Tutorial::ShowConsumeText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"�\���L�[��ŃA�C�e�����g�p");
	}
	else if (tutorial.GetFlag(Tutorial::ShowEnchantText) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"�\���L�[���ŕ���ɃG���`�����g");
	}
	else if (tutorial.GetFlag(Tutorial::ShowEnchantText2) == Tutorial::Displaying)
	{
		DrawMessage(dc, L"�e�U���ɃG���`�����g�������ǉ����ʁ@���x���g�p����ƃG���`�����g���ʂ͏�����");
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

	// �e�L�X�g�t���[���̕`��
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
	
	// �e�L�X�g�̕`��
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
