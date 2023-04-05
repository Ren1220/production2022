#pragma once
// http://wisdom.sakura.ne.jp/system/winapi/common/common6.html

/* --- ヘッダファイルのインクルード ---- */

#include <Windows.h>
#include <string>

/* --- 宣言 ---- */

class FileDialogBox
{
public:

	enum Mode
	{
		Open = 0, // 開くファイルを選択する
		Save = 1, // 保存するファイルを選択する
	};

	/// <summary>
	/// ダイアログボックスを開く
	/// カレントディレクトリが内部処理によって変化するため注意が必要
	/// </summary>
	/// <param name="mode"></param>
	/// <param name="filter">開くファイルの種類。(表示する名前)\0(実際にフィルタをかける拡張子の種類)\0</param>
	/// <param name="dirlog_dlr">ダイアログボックスを表示する際に開かれるディレクトリ。空の場合は最後に開いたディレクトリ</param>
	/// <returns>ダイアログボックスで選択したファイルパス</returns>
	static std::wstring SelectPathW(
		Mode mode, 
		const wchar_t* filter = L"", 
		const wchar_t* dirlog_dlr = L".")
	{
		constexpr size_t max_path_len{ 260ULL };

		OPENFILENAME ofn;
		std::wstring out_filepath;
		out_filepath.resize(max_path_len);
		// ファイルを開くダイアログを表示するための構造体を初期化
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = out_filepath.data();
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = max_path_len;
		ofn.lpstrFilter = filter; // 選択するファイルのフィルタ
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = dirlog_dlr;

		BOOL result{};

		switch (mode)
		{
		case FileDialogBox::Mode::Open: // 開くファイルを選択する
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			result = GetOpenFileNameW(&ofn);
			break;
		case FileDialogBox::Mode::Save:// 保存するファイルを選択する
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
			result = GetSaveFileNameW(&ofn);
			break;
		default:
			_ASSERT_EXPR(0, L"不明なダイアログモード");
			return L"";
			break;
		}

		_ASSERT_EXPR(result > 0, L"ダイアログでエラー");

		// 余分なデータを切り捨て
		out_filepath.erase(
			out_filepath.begin() + out_filepath.find_first_of((wchar_t)0), // ヌル文字の位置
			out_filepath.end());

		return out_filepath;
	}

	/// <summary>
	/// ダイアログボックスを開く
	/// カレントディレクトリが内部処理によって変化するため注意が必要
	/// </summary>
	/// <param name="mode"></param>
	/// <param name="filter">開くファイルの種類。(表示する名前)\0(実際にフィルタをかける拡張子の種類)\0</param>
	/// <param name="dirlog_dlr">ダイアログボックスを表示する際に開かれるディレクトリ。空の場合は最後に開いたディレクトリ</param>
	/// <returns>ダイアログボックスで選択したファイルパス</returns>
	static std::string SelectPathA(
		Mode mode, 
		const char* filter = "", 
		const char* dirlog_dlr = ".")
	{
		constexpr size_t max_path_len{ 260ULL };

		OPENFILENAMEA ofn;
		std::string out_filepath;
		out_filepath.resize(max_path_len);
		// ファイルを開くダイアログを表示するための構造体を初期化
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = out_filepath.data();
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = max_path_len;
		ofn.lpstrFilter = filter; // 選択するファイルのフィルタ
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = dirlog_dlr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		BOOL result{};

		switch (mode)
		{
		case FileDialogBox::Mode::Open: // 開くファイルを選択する
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			result = GetOpenFileNameA(&ofn);
			break;
		case FileDialogBox::Mode::Save:// 保存するファイルを選択する
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
			result = GetSaveFileNameA(&ofn);
			break;
		default:
			_ASSERT_EXPR(0, L"不明なダイアログモード");
			return "";
			break;
		}

		_ASSERT_EXPR(result > 0, L"ダイアログでエラー");


		// 余分なデータを切り捨て
		out_filepath.erase(
			out_filepath.begin() + out_filepath.find_first_of((char)0), // ヌル文字の位置
			out_filepath.end());

		return out_filepath;
	}

};
