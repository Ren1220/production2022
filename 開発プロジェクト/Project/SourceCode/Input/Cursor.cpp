#include "Cursor.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cassert>

/* --- ��` ---- */

void Cursor::Update(HWND hwnd)
{
	ObtainPosition(hwnd);
}

void Cursor::ObtainPosition(HWND hwnd)
{
	POINT point{};

	// �X�N���[�����ɂ�������W
	GetCursorPos(&point);
	position[1] = position[0]; // �O�̃t���[���̍��W��n���B
	position[0] = point;

	// �E�B���h�E���ɂ�������W
	BOOL succeded{ ScreenToClient(hwnd, &point) };
	_ASSERT_EXPR(succeded, GetLastError());
	position[3] = position[2];
	position[2] = point;
}
