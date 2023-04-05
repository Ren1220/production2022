#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <Windows.h>
#include <WinUser.h>
#include <array>

/* --- �錾 ---- */

class Cursor
{
public:

	Cursor() = default;
	~Cursor() = default;

	void Update(HWND hwnd);

	/// <summary>
	// �X�N���[�����ɂ�������W
	/// </summary>
	const POINT& GetCurrentAbsPosition() const { return position[0]; }
	const POINT& GetPreviousAbsPosition() const { return position[1]; }

	/// <summary>
	// �E�B���h�E���ɂ�������W
	/// </summary>
	const POINT& GetCurrentPosition() const { return position[2]; }
	const POINT& GetPreviousPosition() const { return position[3]; }

private:

	/// <summary>
	/// �J�[�\���̍��W����肷��
	/// </summary>
	void ObtainPosition(HWND hwnd);
	
	// [0][1] abs, [2][3] relative
	std::array<POINT, 4> position;
};
