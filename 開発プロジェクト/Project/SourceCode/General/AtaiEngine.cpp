#include "AtaiEngine.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <string>
#include <vector>

#include "Include/misc.h"

/* --- �֐��̎��� ---- */

Engine::Engine()
	: is_fullscreen(false), window_rect(), window_style(), framebuffer_dimensions(), 
	  swap_chain_present_flag(DXGI_PRESENT_ALLOW_TEARING)
{
	// �_�C�A���O�{�b�N�X���g�p���邱�ƂŃJ�����g�f�B���N�g�����ω����邽�߁A�J�n���̃f�B���N�g���p�X���m�ۂ��Ă����B
	std::string dir;
	dir.resize(260);
	GetCurrentDirectoryA(static_cast<DWORD>(dir.size()), dir.data());
	dir.erase(dir.begin() + dir.find_first_of((char)0), dir.end());
	project_directory = dir;
}

void Engine::Initialize(const HWND hwnd, bool is_fullscreen)
{
	HRESULT hr{ S_OK };

	this->hwnd = hwnd;

	this->is_fullscreen = is_fullscreen;

	if (is_fullscreen)
	{
		SetupFullScreen();
	}

	// �t���[���o�b�t�@�̃T�C�Y���v�Z����
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	framebuffer_dimensions.cx = client_rect.right - client_rect.left;
	framebuffer_dimensions.cy = client_rect.bottom - client_rect.top;

	// ���݂̃E�B���h�E�̏�ԁi�����̏�Ԃ��j�ۑ�����
	window_style = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));

	// DirectX������
	directx.Initialize(hwnd, framebuffer_dimensions.cx, framebuffer_dimensions.cy);

	swap_chain_present_flag = DXGI_PRESENT_ALLOW_TEARING;
}

void Engine::ActivateSwapChainBuffer()
{
	auto [wf, hf] { GetScreenSize<float>() };
	directx.ActivateSwapChainBuffer(wf, hf);
}

void Engine::Present()
{
	directx.Present(0, swap_chain_present_flag);
}

void Engine::Resize(UINT width, UINT height)
{
	/* --- ���� ---- */

	HRESULT hr{ S_OK };

	auto [uw, uh] { GetScreenSize<UINT>() };

	// �E�B���h�E�̃T�C�Y���قȂ�ꍇ
	if (width != uw || height != uh )
	{
		framebuffer_dimensions.cx = static_cast<LONG>(width);
		framebuffer_dimensions.cy = static_cast<LONG>(height);

		// DirectX�n�̍Đݒ�
		directx.Resize(
			static_cast<UINT>(framebuffer_dimensions.cx),
			static_cast<UINT>(framebuffer_dimensions.cy));

		// �X�N���[���̍Đݒ���s��
		SetupWindow();
	}
}

void Engine::ChangeScreenMode(bool is_fullscreen)
{
	if (this->is_fullscreen != is_fullscreen)
	{
		this->is_fullscreen = is_fullscreen;

		// �X�N���[�����[�h���ύX���ꂽ�ׁA
		// �X�N���[���̍Đݒ���s��
		SetupFullScreen();
	}
}

void Engine::RestoreCurrentDirctory() const
{
	SetCurrentDirectoryA(project_directory.c_str());

	char a[100]{ };
	GetCurrentDirectoryA(100, a);
	OutputDebugStringA(a);
}

void Engine::SetSwapChainPresentFlag(UINT flag)
{
	swap_chain_present_flag = flag;
}

void Engine::SetupFullScreen()
{
	/* --- ���� ---- */

	if (is_fullscreen)
	{
		// �t���X�N���[�����[�h�̎��̐ݒ�

		// �E�B���h�E�̃T�C�Y�̎擾
		GetWindowRect(hwnd, &window_rect);

		// �E�B���h�E�̏�Ԃ�ύX����
		SetWindowLongPtrA(
			hwnd,			// �E�B���h�E�̃n���h���� 
			GWL_STYLE,		// �E�B���h�E�̏�Ԃ�V�����ݒ肷��t���O
			//https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
			WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
		);

		// �t���X�N���[�����[�h�̃E�B���h�E�̃T�C�Y
		RECT fullscreen_window_rect;

		DEVMODE devmode{};
		devmode.dmSize = sizeof(DEVMODE);
		// https://www.tokovalue.jp/function/EnumDisplaySettings.htm
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

		// �t���X�N���[�����[�h�̃E�B���h�E�̃T�C�Y��ݒ肷��
		fullscreen_window_rect =
		{
			devmode.dmPosition.x,
			devmode.dmPosition.y,
			devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
			devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
		};

		// �E�B���h�E�̃T�C�Y��ݒ肷�� https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
		SetWindowPos(
			hwnd,
#ifdef _DEBUG
			NULL,
#else
			HWND_TOPMOST,
#endif // _DEBUG
			fullscreen_window_rect.left,
			fullscreen_window_rect.top,
			fullscreen_window_rect.right,
			fullscreen_window_rect.bottom,
			SWP_FRAMECHANGED | // SetWindowLong�֐��Őݒ肵���V�����E�B���h�E�X�^�C����K�p����
			SWP_NOACTIVATE	   // �E�B���h�E���A�N�e�B�u��Ԃɂ��Ȃ�
		);

		// �E�B���h�E��\������ https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
		ShowWindow(
			hwnd,
			SW_MAXIMIZE // �E�B���h�E���ő剻������Ԃŕ\������
		);
	}
	else
	{
		SetWindowLongPtrA(hwnd, GWL_STYLE, window_style);
		SetWindowPos(
			hwnd,
			HWND_NOTOPMOST,
			window_rect.left,
			window_rect.top,
			window_rect.right - window_rect.left,
			window_rect.bottom - window_rect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd, SW_NORMAL);
	}
}

void Engine::SetupWindow()
{
	// �E�B���h�E�̃T�C�Y�̎擾
	GetWindowRect(hwnd, &window_rect);

	SetWindowLongPtrA(hwnd, GWL_STYLE, window_style);
	SetWindowPos(
		hwnd,
		HWND_NOTOPMOST,
		window_rect.left,
		window_rect.top,
		window_rect.left + framebuffer_dimensions.cx,
		window_rect.top + framebuffer_dimensions.cy,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);

	ShowWindow(hwnd, SW_NORMAL);
}
