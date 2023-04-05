#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <fstream>
#include <filesystem>
#include <vector>

/* --- �錾 ---- */

class FileLoader
{
public:
	/// <summary>
	/// �����̃f�B���N�g�����̃p�X�����ׂĎ擾����
	/// </summary>
	/// <param name="directorypath">�f�B���N�g���̃p�X</param>
	/// <param name="is_create_directories">�f�B���N�g�������݂��Ȃ������Ƃ��V�����f�B���N�g���𐶐����邩</param>
	/// <returns>�f�B���N�g���p�X�̔z��</returns>
	static std::vector<std::filesystem::path> OpenDirectory(const char* directorypath, bool is_create_directories = false)
	{
		std::filesystem::directory_entry directory(directorypath);
		std::vector<std::filesystem::path> buffer{};

		// �����̃f�B���N�g�������݂��Ȃ�
		if (!directory.exists())
		{
			if (is_create_directories)
			{
				std::filesystem::create_directories(directorypath);
			}
			else
			{
				// ��̂܂ܕԂ�
				buffer;
			}
		}

		std::filesystem::directory_iterator directory_itr(directorypath);

		// �f�B���N�g�����̃p�X���擾����
		for (const std::filesystem::directory_entry& child : directory_itr)
		{
			buffer.emplace_back(child.path());
		}

		return buffer;
	}
};

