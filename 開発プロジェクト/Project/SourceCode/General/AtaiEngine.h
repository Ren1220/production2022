#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

// �}���`�X���b�h���ɓ����ϐ��ɃA�N�Z�X�����Ȃ����߂̏������s��
#include <mutex> 

#include <stdint.h>
#include <filesystem>

#include "Utility/Singleton.h"
#include "Graphics/General/DirectXUser.h"


/* --- �N���X�̒�` ---- */

/// <summary>
/// DirectX�S��
/// </summary>
class Engine : public Singleton<Engine>
{
public:

	Engine();
	~Engine() = default;

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="set_hwnd"></param>
	/// <param name="is_fullscreen">�X�N���[�����[�h</param>
	void Initialize(const HWND set_hwnd, bool is_fullscreen);
	/// <summary>
	/// �X���b�v�`�F�[���̃o�b�t�@���f�o�C�X�R���e�L�X�g�ɐݒ肷��B
	/// </summary>
	void ActivateSwapChainBuffer();
	/// <summary>
	/// �E�B���h�E�ɕ`�悷��
	/// </summary>
	void Present();
	/// <summary>
	/// �X�N���[���T�C�Y�̕ύX
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void Resize(UINT width, UINT height);
	/// <summary>
	/// �X�N���[�����[�h�̕ύX
	/// </summary>
	void ChangeScreenMode(bool is_fullscreen);

	/// <summary>
	/// �J�����g�f�B���N�g�����ω������ۂɊJ�n���̃J�����g�f�B���N�g���ɖ߂����߂Ɏg�p����B�@
	/// </summary>
	void RestoreCurrentDirctory() const;

	ID3D11Device* GetDevice() { return directx.GetDevice(); }
	ID3D11DeviceContext* GetDeviceContext() { return directx.GetDeviceContext(); }
	ID2D1DeviceContext* GetDeviceContext2D() { return directx.GetDeviceContext2D(); }
	ID2D1Factory1* GetFactory2D() { return directx.GetFactory2D(); }
	std::mutex& GetMutex() { return mutex; }

	bool IsFullscreen() const { return is_fullscreen; }

	template <class T>
	std::pair<T, T> GetScreenSize() const
	{
		return { static_cast<T>(framebuffer_dimensions.cx), static_cast<T>(framebuffer_dimensions.cy) };
	}

	/// <summary>
	/// �`�掞�̐ݒ�
	/// </summary>
	/// <param name="flag">DXGI_PRESENT_ALLOW_TEARING��fps�̏������������</param>
	void SetSwapChainPresentFlag(UINT flag);

private:

	/// <summary>
	/// �t���X�N���[���A�E�B���h�E�̐؂�ւ��ݒ�
	/// </summary>
	void SetupFullScreen();

	void SetupWindow();

private:

	HWND hwnd = NULL;
	std::mutex mutex;
	bool is_fullscreen;
	RECT window_rect;
	DWORD window_style;
	SIZE framebuffer_dimensions;

	DirectXUser directx;
	UINT swap_chain_present_flag; // DXGI_PRESENT_ALLOW_TEARING��fps�̏������������

	std::string project_directory; // �_�C�A���O�{�b�N�X���g�p���邱�ƂŃJ�����g�f�B���N�g�����ω����邽�߁A�J�n���̃f�B���N�g���p�X���m�ۂ��Ă����B

};

/* --- �}�N���̒�` ---- */

#define MySystem (Engine::GetInstance())