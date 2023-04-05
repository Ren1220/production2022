#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <fstream>
#include <filesystem>
#include <vector>

/* --- 宣言 ---- */

class FileLoader
{
public:
	/// <summary>
	/// 引数のディレクトリ内のパスをすべて取得する
	/// </summary>
	/// <param name="directorypath">ディレクトリのパス</param>
	/// <param name="is_create_directories">ディレクトリが存在しなかったとき新しくディレクトリを生成するか</param>
	/// <returns>ディレクトリパスの配列</returns>
	static std::vector<std::filesystem::path> OpenDirectory(const char* directorypath, bool is_create_directories = false)
	{
		std::filesystem::directory_entry directory(directorypath);
		std::vector<std::filesystem::path> buffer{};

		// 引数のディレクトリが存在しない
		if (!directory.exists())
		{
			if (is_create_directories)
			{
				std::filesystem::create_directories(directorypath);
			}
			else
			{
				// 空のまま返す
				buffer;
			}
		}

		std::filesystem::directory_iterator directory_itr(directorypath);

		// ディレクトリ内のパスを取得する
		for (const std::filesystem::directory_entry& child : directory_itr)
		{
			buffer.emplace_back(child.path());
		}

		return buffer;
	}
};

