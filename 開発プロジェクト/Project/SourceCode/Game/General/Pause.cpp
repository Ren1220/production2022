#include "Pause.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Audio/AudioManager.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"
#include "Graphics/PostProcess/PostEffect.h"
#include "General/AtaiEngine.h"
#include "Game/General/GameSystem.h"
#include "Game/General/Inventory.h"
#include "Game/General/GameText.h"
#include "Game/Stage/StageManager.h"
#include "Game/Actor/ActorItem.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Component/Item/ItemLifePotion.h"
#include "Game/Component/Item/ItemFirePotion.h"
#include "Game/Component/Item/ItemSparkPotion.h"
#include "Input/Input.h"
#include "Utility/UserFunc.h"

/* --- ��` ---- */

PauseSystem::PauseSystem()
	: is_pause(false), select_menu_index(0), show_back(false), is_control_pitch(false), is_change_screen(false)
{
}

PauseSystem::~PauseSystem()
{
}

void PauseSystem::Initialize(ID3D11Device* device)
{
	menu_back.Create(device,		L"Data/Texture/Pause/menu_back.png");
	menu_control.Create(device,		L"Data/Texture/Pause/control.png");
	menu_bar.Create(device,			L"Data/Texture/Pause/bar.png");
	menu_bar_pitch.Create(device,	L"Data/Texture/Pause/bar_pitch.png");
	select_item_back.Create(device);
}

void PauseSystem::Update(float elapsed_time, float width, float height)
{
	// �|�[�Y��Ԃ̂ݍX�V
	if (!is_pause) return;

	show_back = true;

	if(menu) menu(elapsed_time, width, height);
}

void PauseSystem::Render(ID3D11DeviceContext* dc, float width, float height)
{
	// �|�[�Y��Ԃ̂ݕ`��
	if (!is_pause) return;

	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };

	if (show_back)
	{
		texture_renderer->BeginDraw(dc, BSMode::Alpha);
		texture_renderer->Draw(dc, menu_back);
		texture_renderer->EndDraw(dc);
	}

	if (draw_menu) draw_menu(dc, width, height);
}

bool PauseSystem::WaitForPauseKey(uint16_t key)
{
	// �����̃L�[���͂Ń|�[�Y��Ԃ�On/Off
	if (Input::GetInstance().GetGamePad().
		GetButton(static_cast<GamePad::Button>(key), GamePad::Flag::DOWN))
	{
		// ���]
		is_pause = !is_pause;

		if (is_pause)
		{
			SetMenuFunc(&PauseSystem::MainMenu);
			SetDrawMenuFunc(&PauseSystem::DrawMain);
		}

		return true;
	}

	return false;
}

void PauseSystem::MenuControl(int32_t menu_item_count, bool is_vertical)
{
	float l{ is_vertical ? 
		Input::GetInstance().GetGamePad().GetStickLY() :
		Input::GetInstance().GetGamePad().GetStickLX() };

	// ���͂̐���
	static int stick_control_flag[2]{ 0, 0 };

	// ��i�E�j�ɓ���,
	if (l > 0.0f)
	{
		if (is_vertical)
		{
			stick_control_flag[1] = -1;
		}
		else
		{
			stick_control_flag[1] = 1;
		}
	}
	// ���i���j�ɓ���
	else if (l < 0.0f)
	{
		if (is_vertical)
		{
			stick_control_flag[1] = 1;
		}
		else
		{
			stick_control_flag[1] = -1;
		}
	}
	else
	{
		stick_control_flag[1] = 0;
	}

	// �X�e�B�b�N�|�����ςȂ��œ����Ȃ��悤�ɂ���
	if (stick_control_flag[0] != stick_control_flag[1])
	{
		select_menu_index += stick_control_flag[1];

		// �C���f�b�N�X�͈̔͂Ɏ��߂�
		select_menu_index =	std::clamp(select_menu_index, 0, menu_item_count - 1);
	}

	stick_control_flag[0] = stick_control_flag[1];
}

void PauseSystem::MainMenu(float elapsed_time, float width, float height)
{
	MenuControl(5, true);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0:
			is_pause = false;
			return;
		case 1:
			SetMenuFunc(&PauseSystem::GoBossMenu, 1);
			SetDrawMenuFunc(&PauseSystem::DrawGoBoss);
			return;
		case 2:
			SetMenuFunc(&PauseSystem::ControlMenu);
			SetDrawMenuFunc(&PauseSystem::DrawControl);
			return;
		case 3:
			SetMenuFunc(&PauseSystem::OptionMenu);
			SetDrawMenuFunc(&PauseSystem::DrawOption);
			return;
		case 4:
			SetMenuFunc(&PauseSystem::GoTitleMenu, 1);
			SetDrawMenuFunc(&PauseSystem::DrawGoTitle);
			return;
		}
	}


	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		is_pause = false;
		return;
	}
}

void PauseSystem::GoBossMenu(float elapsed_time, float width, float height)
{
	MenuControl(2, false);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0:
		{
			// �C���x���g���̏�����
			GameSys.GetInventory()->Initialize();

			// �񕜃A�C�e���Q�A�G���`�����g�A�C�e�����Q�����������
			for(int i = 0; i < 2; ++i)
			{
				auto actor{ GameSys.GetActorManager()->Create<ActorItem>() };
				actor->Initialize(MySystem.GetDevice(), std::format("HelpItem{}", i).c_str(), {});
				actor->AddComponent<ItemLifePotion>(100);
				GameSys.GetInventory()->AddItem(actor);
			}

			{
				auto fire{ GameSys.GetActorManager()->Create<ActorItem>() };
				auto spark{ GameSys.GetActorManager()->Create<ActorItem>() };
				fire->Initialize(MySystem.GetDevice(),  "HelpItem2", {});
				spark->Initialize(MySystem.GetDevice(), "HelpItem3", {});
				fire->AddComponent<ItemFirePotion>();
				spark->AddComponent<ItemSparkPotion>();
				GameSys.GetInventory()->AddItem(fire);
				GameSys.GetInventory()->AddItem(spark);
			}

			// �X�e�[�W�̕ύX
			GameSys.GetStageManager()->NextStage(static_cast<uint32_t>(StageManager::ExtraStageIndex::Boss));
			
			// ���j���[�����
			is_pause = false;

			return;
		}
		case 1:
			SetMenuFunc(&PauseSystem::MainMenu, 1);
			SetDrawMenuFunc(&PauseSystem::DrawMain);
			return;
		}

	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		SetMenuFunc(&PauseSystem::MainMenu, 1);
		SetDrawMenuFunc(&PauseSystem::DrawMain);
		return;
	}
}

void PauseSystem::ControlMenu(float elapsed_time, float width, float height)
{
	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN) ||
		Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		SetMenuFunc(&PauseSystem::MainMenu, 2);
		SetDrawMenuFunc(&PauseSystem::DrawMain);
		return;
	}
}

void PauseSystem::OptionMenu(float elapsed_time, float width, float height)
{

	MenuControl(3, true);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0:
			SetMenuFunc(&PauseSystem::DisplayMenu);
			SetDrawMenuFunc(&PauseSystem::DrawDisplay);
			return;
		case 1:
			SetMenuFunc(&PauseSystem::SoundMenu);
			SetDrawMenuFunc(&PauseSystem::DrawSound);
			return;
		case 2:
			SetMenuFunc(&PauseSystem::MainMenu, 3);
			SetDrawMenuFunc(&PauseSystem::DrawMain);
			return;
		}
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		SetMenuFunc(&PauseSystem::MainMenu, 3);
		SetDrawMenuFunc(&PauseSystem::DrawMain);
		return;
	}
}

void PauseSystem::GoTitleMenu(float elapsed_time, float width, float height)
{
	MenuControl(2, false);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0:
		{
			// �Q�[���I�[�o�[
			GameSys.GameOver();
			// ���j���[�����
			is_pause = false;
			return;
		}
		case 1:
			SetMenuFunc(&PauseSystem::MainMenu, 4);
			SetDrawMenuFunc(&PauseSystem::DrawMain);
			return;
		}
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		SetMenuFunc(&PauseSystem::MainMenu, 4);
		SetDrawMenuFunc(&PauseSystem::DrawMain);
		return;
	}
}

void PauseSystem::SoundMenu(float elapsed_time, float width, float height)
{
	MenuControl(4, true);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0: // �}�X�^
			is_control_pitch = true;
			SetMenuFunc(&PauseSystem::SoundMasteringVoice, 0);
			return;
		case 1: // BGM
			is_control_pitch = true;
			SetMenuFunc(&PauseSystem::SoundBGM, 1);
			return;
		case 2: // SE
			is_control_pitch = true;
			SetMenuFunc(&PauseSystem::SoundSE, 2);
			return;
		case 3: // �߂�
			SetMenuFunc(&PauseSystem::OptionMenu, 1);
			SetDrawMenuFunc(&PauseSystem::DrawOption);
			return;
		}
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		SetMenuFunc(&PauseSystem::OptionMenu, 1);
		SetDrawMenuFunc(&PauseSystem::DrawOption);
		return;
	}
}

void PauseSystem::SoundMasteringVoice(float elapsed_time, float width, float height)
{
	is_control_pitch = true;

	float lx{ Input::GetInstance().GetGamePad().GetStickLX() };

	float volume{ AudioManager::GetInstance().GetMasteringVoiceVolume() };
	volume += lx * elapsed_time;
	volume = std::clamp(volume, 0.0f, volume_max);
	AudioManager::GetInstance().SetMasteringVoiceVolume(volume);

	// ������
	if (Input::GetInstance().GetGamePad().GetButton(GamePad::Y, GamePad::DOWN))
	{
		AudioManager::GetInstance().SetMasteringVoiceVolume(1.0f);
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN) ||
		Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		is_control_pitch = false;
		SetMenuFunc(&PauseSystem::SoundMenu, 0);
		return;
	}
}

void PauseSystem::SoundBGM(float elapsed_time, float width, float height)
{
	is_control_pitch = true;

	float lx{ Input::GetInstance().GetGamePad().GetStickLX() };

	float volume{ AudioManager::GetInstance().GetBGMVolume() };
	volume += lx * elapsed_time;
	volume = std::clamp(volume, 0.0f, volume_max);
	AudioManager::GetInstance().SetBGMVolume(volume);

	// ������
	if (Input::GetInstance().GetGamePad().GetButton(GamePad::Y, GamePad::DOWN))
	{
		AudioManager::GetInstance().SetBGMVolume(1.0f);
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN) ||
		Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		is_control_pitch = false;
		SetMenuFunc(&PauseSystem::SoundMenu, 1);
		return;
	}
}

void PauseSystem::SoundSE(float elapsed_time, float width, float height)
{
	is_control_pitch = true;

	float lx{ Input::GetInstance().GetGamePad().GetStickLX() };

	float volume{ AudioManager::GetInstance().GetSEVolume() };
	volume += lx * elapsed_time;
	volume = std::clamp(volume, 0.0f, volume_max);
	AudioManager::GetInstance().SetSEVolume(volume);

	// ������
	if (Input::GetInstance().GetGamePad().GetButton(GamePad::Y, GamePad::DOWN))
	{
		AudioManager::GetInstance().SetSEVolume(1.0f);
	}

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN) ||
		Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		is_control_pitch = false;
		SetMenuFunc(&PauseSystem::SoundMenu, 2);
		return;
	}
}

void PauseSystem::DisplayMenu(float elapsed_time, float width, float height)
{
	MenuControl(3, true);

	if (Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		switch (select_menu_index)
		{
		case 0: // �t���X�N���[���ݒ�
			is_change_screen = true;
			return;
		case 1: // ���邳�ݒ�
			SetMenuFunc(&PauseSystem::DisplayControlBrightness);
			is_control_pitch = true;
			return;
		case 2:
			show_back = true;
			SetMenuFunc(&PauseSystem::OptionMenu, 0);
			SetDrawMenuFunc(&PauseSystem::DrawOption);
			return;
		}
	}


	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN))
	{
		show_back = true;
		SetMenuFunc(&PauseSystem::OptionMenu, 0);
		SetDrawMenuFunc(&PauseSystem::DrawOption);
		return;
	}
}

void PauseSystem::DisplayControlBrightness(float elapsed_time, float width, float height)
{
	float lx{ Input::GetInstance().GetGamePad().GetStickLX() };

	show_back = false;

	auto filter_param{ Graphics::GetInstance().GetPostEffect()->GetColorFilter().GetFilterParam() };

	float brightness{ filter_param.brightness };

	brightness += lx * elapsed_time;
	brightness = std::clamp(brightness, 0.0f, bright_max);

	filter_param.brightness = brightness;

	// ������
	if (Input::GetInstance().GetGamePad().GetButton(GamePad::Y, GamePad::DOWN))
	{
		filter_param.brightness = 1.0f;
	}

	Graphics::GetInstance().GetPostEffect()->GetColorFilter().SetFilterParam(filter_param);


	if (Input::GetInstance().GetGamePad().GetButton(GamePad::B, GamePad::DOWN) ||
		Input::GetInstance().GetGamePad().GetButton(GamePad::A, GamePad::DOWN))
	{
		is_control_pitch = false;
		SetMenuFunc(&PauseSystem::DisplayMenu, 0);
		return;
	}
}

void PauseSystem::DrawMain(ID3D11DeviceContext* dc, float width, float height)
{
	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };

	constexpr const wchar_t* headline{ L"PAUSE" };

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt,
		headline,
		FontFamily::Kokumincho,
		font_size * 2.0f,
		{ 0.0f, (height / 2.0f - 300.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	constexpr std::array<const wchar_t*, 5ULL> menu_texts
	{
		L"�Q�[���ɖ߂�",
		L"�{�X�t���A��",
		L"������@",
		L"�I�v�V����",
		L"�^�C�g���ɖ߂�"
	};

	for (int i = 0; i < menu_texts.size(); ++i)
	{
		game_text->SetText(
			GameText::Layer::Opt,
			menu_texts[i],
			FontFamily::Kokumincho,
			font_size * ((i == select_menu_index) ? 1.5f : 1.0f),
			{ 0.0f, (height / 2.0f - 150.0f) + 100.0f * i },
			{ width, 100.0f },
			{ 1, 1, 1, 1 },
			DWRITE_TEXT_ALIGNMENT_CENTER,
			DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void PauseSystem::DrawGoBoss(ID3D11DeviceContext* dc, float width, float height)
{
	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };

	constexpr auto headline{ L"�{�X�t���A��" };
	constexpr auto text{ L"�A�C�e���𕡐�����������ԂŃ{�X�t���A�ֈړ����܂��B" };

	constexpr std::array<const wchar_t*, 2ULL> selects{ L"�͂�", L"������" };

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt, headline,
		FontFamily::Kokumincho,
		font_size * 1.5f,
		{ 0.0f, (height / 2.0f - 200.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �e�L�X�g
	game_text->SetText(
		GameText::Layer::Opt, text,
		FontFamily::Kokumincho,
		font_size * 0.75f,
		{ 0.0f, (height / 2.0f - 200.0f) + 100.0f },
		{ width, 100.0f },
		{ 1, 1, 1, 0.8f },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �͂�
	game_text->SetText(
		GameText::Layer::Opt, selects[0],
		FontFamily::Kokumincho,
		font_size * ((select_menu_index == 0) ? 1.5f : 1.0f),
		{ width / 2.0f - 150.0f, (height / 2.0f) },
		{ 150.0f, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// ������
	game_text->SetText(
		GameText::Layer::Opt, selects[1],
		FontFamily::Kokumincho,
		font_size * ((select_menu_index == 1) ? 1.5f : 1.0f),
		{ width / 2.0f, (height / 2.0f) },
		{ 150.0f, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void PauseSystem::DrawControl(ID3D11DeviceContext* dc, float width, float height)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };
	texture_renderer->BeginDraw(dc, BSMode::Alpha);
	texture_renderer->Draw(dc, menu_control);
	texture_renderer->EndDraw(dc);
}

void PauseSystem::DrawOption(ID3D11DeviceContext* dc, float width, float height)
{
	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };

	constexpr auto headline{ L"�I�v�V����" };

	constexpr std::array<const wchar_t*, 3ULL> menu_texts
	{
		L"�f�B�X�v���C�ݒ�",
		L"�T�E���h�ݒ�",
		L"�߂�"
	};

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt, headline,
		FontFamily::Kokumincho,
		font_size * 2.0f,
		{ 0.0f, (height / 2.0f - 400.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	for (int i = 0; i < menu_texts.size(); ++i)
	{
		game_text->SetText(
			GameText::Layer::Opt, menu_texts[i],
			FontFamily::Kokumincho,
			font_size * ((i == select_menu_index) ? 1.5f : 1.0f),
			{ 0.0f, (height / 2.0f - 200.0f) + 100.0f * i },
			{ width, 100.0f },
			{ 1, 1, 1, 1 },
			DWRITE_TEXT_ALIGNMENT_CENTER,
			DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void PauseSystem::DrawGoTitle(ID3D11DeviceContext* dc, float width, float height)
{
	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };

	constexpr auto headline{ L"�^�C�g���ɖ߂�" };
	constexpr auto text{ L"�Q�[���̐i�s�󋵂����Z�b�g����܂��B��낵���ł����H" };

	constexpr std::array<const wchar_t*, 2ULL> selects{ L"�͂�", L"������" };

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt, headline,
		FontFamily::Kokumincho,
		font_size * 1.5f,
		{ 0.0f, (height / 2.0f - 200.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �e�L�X�g
	game_text->SetText(
		GameText::Layer::Opt, text,
		FontFamily::Kokumincho,
		font_size * 0.75f,
		{ 0.0f, (height / 2.0f - 200.0f) + 100.0f },
		{ width, 100.0f },
		{ 1, 1, 1, 0.8f },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �͂�
	game_text->SetText(
		GameText::Layer::Opt, selects[0],
		FontFamily::Kokumincho,
		font_size * ((select_menu_index == 0) ? 1.5f : 1.0f),
		{ width / 2.0f - 150.0f, (height / 2.0f) },
		{ 150.0f, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// ������
	game_text->SetText(
		GameText::Layer::Opt, selects[1],
		FontFamily::Kokumincho,
		font_size * ((select_menu_index == 1) ? 1.5f : 1.0f),
		{ width / 2.0f, (height / 2.0f) },
		{ 150.0f, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void PauseSystem::DrawSound(ID3D11DeviceContext* dc, float width, float height)
{
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };

	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };

	constexpr auto headline{ L"�T�E���h�ݒ�" };

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt, headline,
		FontFamily::Kokumincho,
		font_size * 1.5f,
		{ 0.0f, (height / 2.0f - 400.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �}�X�^�{�����[��
	{
		float volume{ AudioManager::GetInstance().GetMasteringVoiceVolume() };
		volume = std::clamp(volume, 0.0f, volume_max);
		float ratio{ volume / volume_max };

		const float w{ width - 600.0f };
		const float y{ height / 2.0f - 200.0f };
		constexpr float x{ 400.0f };

		texture_renderer->BeginDraw(dc);

		if (select_menu_index == 0 && is_control_pitch)
		{
			texture_renderer->Draw(dc, select_item_back, { x - 10.0f, y - 50.0f }, { w + 10.0f, 100.0f }, { 1, 1, 1, 0.3f }, TexturePivot::LeftTop);
		}

		texture_renderer->Draw(dc, menu_bar, { x, y }, { w,  5.0f }, { 1, 1, 1, 1 }, TexturePivot::LeftTop);
		texture_renderer->Draw(dc, menu_bar_pitch, { x + w * ratio, y }, TexturePivot::Center);
		texture_renderer->EndDraw(dc);

		// ���ږ�
		game_text->SetText(
			GameText::Layer::Opt, L"Master", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 0 ? 1.5f : 1.0f), { 0.0f, y - 20.0f }, { x, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		// ����
		if (select_menu_index == 0 && is_control_pitch)
		{
			game_text->SetText(
				GameText::Layer::Opt, L"Y�������Ə����l�ɖ߂�", FontFamily::Kokumincho, font_size * 0.75f, { 0.0f, y - 100.0f }, { w, 20.0f }, { 1.0f, 1.0f, 1.0f, 0.8f }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		// ���݂̒l
		const auto percent{ std::format(L"{:.0f}%", ratio * 200.0f) };
		game_text->SetText(
			GameText::Layer::Opt, percent.c_str(), FontFamily::Kokumincho, font_size, { x, y }, { w + 10.0f, 20.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING);
	}

	// BGM�{�����[��
	{
		float volume{ AudioManager::GetInstance().GetBGMVolume() };
		volume = std::clamp(volume, 0.0f, volume_max);
		float ratio{ volume / volume_max };

		const float w{ width - 600.0f };
		const float y{ height / 2.0f  };
		constexpr float x{ 400.0f };

		texture_renderer->BeginDraw(dc);

		if (select_menu_index == 1 && is_control_pitch)
		{
			texture_renderer->Draw(dc, select_item_back, { x - 10.0f, y - 50.0f }, { w + 10.0f, 100.0f }, { 1, 1, 1, 0.3f }, TexturePivot::LeftTop);
		}

		texture_renderer->Draw(dc, menu_bar, { x, y }, { w,  5.0f }, { 1, 1, 1, 1 }, TexturePivot::LeftTop);
		texture_renderer->Draw(dc, menu_bar_pitch, { x + w * ratio, y }, TexturePivot::Center);
		texture_renderer->EndDraw(dc);

		// ���ږ�
		game_text->SetText(
			GameText::Layer::Opt, L"BGM", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 1 ? 1.5f : 1.0f), { 0.0f, y - 20.0f }, { x, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		// ����
		if (select_menu_index == 1 && is_control_pitch)
		{
			game_text->SetText(
				GameText::Layer::Opt, L"Y�������Ə����l�ɖ߂�", FontFamily::Kokumincho, font_size * 0.75f, { 0.0f, y - 100.0f }, { w, 20.0f }, { 1.0f, 1.0f, 1.0f, 0.8f }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		// ���݂̒l
		const auto percent{ std::format(L"{:.0f}%", ratio * 200.0f) };
		game_text->SetText(
			GameText::Layer::Opt, percent.c_str(), FontFamily::Kokumincho, font_size, { x, y }, { w + 10.0f, 20.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING);
	}

	// SE
	{
		float volume{ AudioManager::GetInstance().GetSEVolume() };
		volume = std::clamp(volume, 0.0f, volume_max);
		float ratio{ volume / volume_max };

		const float w{ width - 600.0f };
		const float y{ height / 2.0f + 200.0f };
		constexpr float x{ 400.0f };

		texture_renderer->BeginDraw(dc);

		if (select_menu_index == 2 && is_control_pitch)
		{
			texture_renderer->Draw(dc, select_item_back, { x - 10.0f, y - 50.0f }, { w + 10.0f, 100.0f }, { 1, 1, 1, 0.3f }, TexturePivot::LeftTop);
		}

		texture_renderer->Draw(dc, menu_bar, { x, y }, { w,  5.0f }, { 1, 1, 1, 1 }, TexturePivot::LeftTop);
		texture_renderer->Draw(dc, menu_bar_pitch, { x + w * ratio, y }, TexturePivot::Center);
		texture_renderer->EndDraw(dc);

		// ���ږ�
		game_text->SetText(
			GameText::Layer::Opt, L"SE", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 2 ? 1.5f : 1.0f), { 0.0f, y - 20.0f }, { x, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		// ����
		if (select_menu_index == 2 && is_control_pitch)
		{
			game_text->SetText(
				GameText::Layer::Opt, L"Y�������Ə����l�ɖ߂�", FontFamily::Kokumincho, font_size * 0.75f, { 0.0f, y - 100.0f }, { w, 20.0f }, { 1.0f, 1.0f, 1.0f, 0.8f }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		// ���݂̒l
		const auto percent{ std::format(L"{:.0f}%", ratio * 200.0f) };
		game_text->SetText(
			GameText::Layer::Opt, percent.c_str(), FontFamily::Kokumincho, font_size, { x, y }, { w + 10.0f, 20.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING);
	}

	// �߂�
	{
		game_text->SetText(
			GameText::Layer::Opt, L"�߂�", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 3 ? 1.5f : 1.0f), { 0.0f, height - 100.0f }, { width - 40.0f, 80.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void PauseSystem::DrawDisplay(ID3D11DeviceContext* dc, float width, float height)
{
	constexpr float font_size{ 30.0f };

	auto& game_text{ GameSys.GetGameText() };
	auto& texture_renderer{ Graphics::GetInstance().GetTextureRenderer() };

	constexpr auto headline{ L"�f�B�X�v���C�ݒ�" };

	// ���o��
	game_text->SetText(
		GameText::Layer::Opt, headline,
		FontFamily::Kokumincho,
		font_size * 1.5f,
		{ 0.0f, (height / 2.0f - 400.0f) },
		{ width, 100.0f },
		{ 1, 1, 1, 1 },
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �X�N���[���ݒ�
	{
		const float w{ width - 600.0f };
		const float y{ height / 2.0f - 200.0f };
		constexpr float x{ 400.0f };
		// ���ږ�
		game_text->SetText(
			GameText::Layer::Opt, L"�X�N���[�����[�h", FontFamily::Kokumincho, font_size * 1.5f *(select_menu_index == 0 ? 1.5f : 1.0f), { 0.0f, y - 20.0f }, { x, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (MySystem.IsFullscreen())
		{
			game_text->SetText(
				GameText::Layer::Opt, L"�t���X�N���[��", FontFamily::Kokumincho, font_size * 1.5f, { x + 100.0f, y - 20.0f }, { w, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
		else
		{
			game_text->SetText(
				GameText::Layer::Opt, L"�E�B���h�E", FontFamily::Kokumincho, font_size * 1.5f, { x + 100.0f, y - 20.0f }, { w, 40.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}

	// ���邳�ݒ�
	{
		const auto& filter_param{ Graphics::GetInstance().GetPostEffect()->GetColorFilter().GetFilterParam() };
		float brightness{ filter_param.brightness };
		brightness = std::clamp(brightness, 0.0f, bright_max);
		float ratio{ brightness / bright_max };

		const float w { width - 600.0f };
		const float y { height / 2.0f};
		constexpr float x{ 400.0f };

		texture_renderer->BeginDraw(dc);

		texture_renderer->Draw(dc, menu_bar, { x, y }, { w,  5.0f }, { 1, 1, 1, 1 }, TexturePivot::LeftTop);
		texture_renderer->Draw(dc, menu_bar_pitch, { x + w * ratio, y }, TexturePivot::Center);
		texture_renderer->EndDraw(dc);

		// ���ږ�
		game_text->SetText(
			GameText::Layer::Opt, L"���邳", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 1 ? 1.5f : 1.0f), {0.0f, y - 20.0f}, {x, 40.0f}, {1, 1, 1, 1}, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		// ����
		if (select_menu_index == 1 && is_control_pitch)
		{
			game_text->SetText(
				GameText::Layer::Opt, L"Y�������Ə����l�ɖ߂�", FontFamily::Kokumincho, font_size * 0.75f, { 0.0f, y - 100.0f }, { w, 20.0f }, { 1.0f, 1.0f, 1.0f, 0.8f }, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		// ���݂̒l
		const auto percent{ std::format(L"{:.0f}%", ratio * bright_max * 100.0f) };
		game_text->SetText(
			GameText::Layer::Opt, percent.c_str(), FontFamily::Kokumincho, font_size, {x, y}, {w + 10.0f, 20.0f}, {1, 1, 1, 1}, DWRITE_TEXT_ALIGNMENT_TRAILING);


	}

	// �߂�
	{
		game_text->SetText(
			GameText::Layer::Opt, L"�߂�", FontFamily::Kokumincho, font_size * 2.0f * (select_menu_index == 2 ? 1.5f : 1.0f), { 0.0f, height - 100.0f }, { width - 40.0f, 80.0f }, { 1, 1, 1, 1 }, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void PauseSystem::SetMenuFunc(void(PauseSystem::* func)(float, float, float), int32_t select_menu_index)
{
	this->select_menu_index = select_menu_index; // ����
	menu = std::bind(func, std::ref(PauseSystem::GetInstance()), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void PauseSystem::SetDrawMenuFunc(void(PauseSystem::* func)(ID3D11DeviceContext*, float, float))
{
	draw_menu = std::bind(func, std::ref(PauseSystem::GetInstance()), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}
