#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <cstdint>
#include <functional>

#include "Utility/Singleton.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- �O���錾 ---- */



/* --- �錾 ---- */

class PauseSystem : public Singleton<PauseSystem>
{
public:

	PauseSystem();
	~PauseSystem();

	void Initialize(ID3D11Device* device);

	void Update(float elapsed_time, float width, float height);

	/// <summary>
	/// �|�[�Y��Ԏ��̕`�揈��
	/// </summary>
	/// <param name="dc"></param>
	void Render(ID3D11DeviceContext* dc, float width, float height);

	/// <summary>
	/// �|�[�Y�L�[���͂�ҋ@���āA���͂��ꂽ��|�[�Y��Ԃ�On/Off���s��
	/// </summary>
	/// <param name="key">���蓖�Ă�|�[�Y�L�[</param>
	/// <returns>�{�^���������ꂽ</returns>
	bool WaitForPauseKey(uint16_t key);

	bool IsPause() const { return is_pause; }

	bool IsChangeScreen() const { return is_change_screen; }
	void ChangedScreen() { is_change_screen = false; }

private:

	void MenuControl(int32_t menu_item_count, bool is_vertical);

	void MainMenu(float elapsed_time, float width, float height);
	void GoBossMenu(float elapsed_time, float width, float height);
	void ControlMenu(float elapsed_time, float width, float height);
	void OptionMenu(float elapsed_time, float width, float height);
	void GoTitleMenu(float elapsed_time, float width, float height);
	void SoundMenu(float elapsed_time, float width, float height);
	void SoundMasteringVoice(float elapsed_time, float width, float height);
	void SoundBGM(float elapsed_time, float width, float height);
	void SoundSE(float elapsed_time, float width, float height);
	void DisplayMenu(float elapsed_time, float width, float height);
	void DisplayControlBrightness(float elapsed_time, float width, float height);

	void DrawMain(ID3D11DeviceContext* dc, float width, float height);
	void DrawGoBoss(ID3D11DeviceContext* dc, float width, float height);
	void DrawControl(ID3D11DeviceContext* dc, float width, float height);
	void DrawOption(ID3D11DeviceContext* dc, float width, float height);
	void DrawGoTitle(ID3D11DeviceContext* dc, float width, float height);
	void DrawSound(ID3D11DeviceContext* dc, float width, float height);
	void DrawDisplay(ID3D11DeviceContext* dc, float width, float height);

	/// <summary>
	/// ���j���[�֐���ݒ肷��
	/// </summary>
	/// <param name="func"></param>
	void SetMenuFunc(void (PauseSystem::* func)(float, float, float), int32_t select_menu_index = 0);

	/// <summary>
	/// ���j���[�`��֐���ݒ肷��
	/// </summary>
	/// <param name="func"></param>
	void SetDrawMenuFunc(void (PauseSystem::* func)(ID3D11DeviceContext*, float, float));

	bool is_pause;
	int32_t select_menu_index;

	std::function<void(float, float, float)> menu;
	std::function<void(ID3D11DeviceContext*, float, float)> draw_menu;

	Texture menu_back;
	Texture menu_control;
	Texture menu_bar;
	Texture menu_bar_pitch;

	Texture select_item_back;


	bool show_back;
	bool is_control_pitch;
	bool is_change_screen;

	static constexpr float bright_max{ 2.0f };
	static constexpr float volume_max{ 2.0f };

};
