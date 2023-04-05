#pragma once
// http://wisdom.sakura.ne.jp/system/winapi/common/common6.html

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <Windows.h>
#include <string>

/* --- �錾 ---- */

class FileDialogBox
{
public:

	enum Mode
	{
		Open = 0, // �J���t�@�C����I������
		Save = 1, // �ۑ�����t�@�C����I������
	};

	/// <summary>
	/// �_�C�A���O�{�b�N�X���J��
	/// �J�����g�f�B���N�g�������������ɂ���ĕω����邽�ߒ��ӂ��K�v
	/// </summary>
	/// <param name="mode"></param>
	/// <param name="filter">�J���t�@�C���̎�ށB(�\�����閼�O)\0(���ۂɃt�B���^��������g���q�̎��)\0</param>
	/// <param name="dirlog_dlr">�_�C�A���O�{�b�N�X��\������ۂɊJ�����f�B���N�g���B��̏ꍇ�͍Ō�ɊJ�����f�B���N�g��</param>
	/// <returns>�_�C�A���O�{�b�N�X�őI�������t�@�C���p�X</returns>
	static std::wstring SelectPathW(
		Mode mode, 
		const wchar_t* filter = L"", 
		const wchar_t* dirlog_dlr = L".")
	{
		constexpr size_t max_path_len{ 260ULL };

		OPENFILENAME ofn;
		std::wstring out_filepath;
		out_filepath.resize(max_path_len);
		// �t�@�C�����J���_�C�A���O��\�����邽�߂̍\���̂�������
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = out_filepath.data();
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = max_path_len;
		ofn.lpstrFilter = filter; // �I������t�@�C���̃t�B���^
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = dirlog_dlr;

		BOOL result{};

		switch (mode)
		{
		case FileDialogBox::Mode::Open: // �J���t�@�C����I������
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			result = GetOpenFileNameW(&ofn);
			break;
		case FileDialogBox::Mode::Save:// �ۑ�����t�@�C����I������
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
			result = GetSaveFileNameW(&ofn);
			break;
		default:
			_ASSERT_EXPR(0, L"�s���ȃ_�C�A���O���[�h");
			return L"";
			break;
		}

		_ASSERT_EXPR(result > 0, L"�_�C�A���O�ŃG���[");

		// �]���ȃf�[�^��؂�̂�
		out_filepath.erase(
			out_filepath.begin() + out_filepath.find_first_of((wchar_t)0), // �k�������̈ʒu
			out_filepath.end());

		return out_filepath;
	}

	/// <summary>
	/// �_�C�A���O�{�b�N�X���J��
	/// �J�����g�f�B���N�g�������������ɂ���ĕω����邽�ߒ��ӂ��K�v
	/// </summary>
	/// <param name="mode"></param>
	/// <param name="filter">�J���t�@�C���̎�ށB(�\�����閼�O)\0(���ۂɃt�B���^��������g���q�̎��)\0</param>
	/// <param name="dirlog_dlr">�_�C�A���O�{�b�N�X��\������ۂɊJ�����f�B���N�g���B��̏ꍇ�͍Ō�ɊJ�����f�B���N�g��</param>
	/// <returns>�_�C�A���O�{�b�N�X�őI�������t�@�C���p�X</returns>
	static std::string SelectPathA(
		Mode mode, 
		const char* filter = "", 
		const char* dirlog_dlr = ".")
	{
		constexpr size_t max_path_len{ 260ULL };

		OPENFILENAMEA ofn;
		std::string out_filepath;
		out_filepath.resize(max_path_len);
		// �t�@�C�����J���_�C�A���O��\�����邽�߂̍\���̂�������
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = out_filepath.data();
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = max_path_len;
		ofn.lpstrFilter = filter; // �I������t�@�C���̃t�B���^
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = dirlog_dlr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		BOOL result{};

		switch (mode)
		{
		case FileDialogBox::Mode::Open: // �J���t�@�C����I������
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			result = GetOpenFileNameA(&ofn);
			break;
		case FileDialogBox::Mode::Save:// �ۑ�����t�@�C����I������
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
			result = GetSaveFileNameA(&ofn);
			break;
		default:
			_ASSERT_EXPR(0, L"�s���ȃ_�C�A���O���[�h");
			return "";
			break;
		}

		_ASSERT_EXPR(result > 0, L"�_�C�A���O�ŃG���[");


		// �]���ȃf�[�^��؂�̂�
		out_filepath.erase(
			out_filepath.begin() + out_filepath.find_first_of((char)0), // �k�������̈ʒu
			out_filepath.end());

		return out_filepath;
	}

};
