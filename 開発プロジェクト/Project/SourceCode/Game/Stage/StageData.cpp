#include "StageData.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <fstream>

/* --- ��` ---- */

void StageData::Export(const char* export_name, const Terrain& terrain)
{
	auto file{ std::fstream(export_name, std::ios::out | std::ios::binary) };

	// �w�b�_���̐ݒ�
	header.width = terrain.GetMapWidth();
	header.height = terrain.GetMapHeight();
	// �}�b�v���̐ݒ�
	FetchData(terrain);

	file.write(reinterpret_cast<char*>(&header), sizeof(Header));
	file.write(reinterpret_cast<char*>(data.data.data()), sizeof(TileData) * data.data.size());
	file.close();
}

void StageData::Import(const char* import_name)
{
	auto file{ std::ifstream(import_name, std::ios::in | std::ios::binary) };

	// �X�e�[�W�̃w�b�_����ǂݎ��
	file.read(reinterpret_cast<char*>(&header), sizeof(Header));

	// �w�b�_��񂩂�}�b�v�T�C�Y���v�Z����B
	int64_t stage_size{ header.width * header.height };
	data.data.resize(stage_size);

	// �}�b�v����ǂݎ��
	file.read(reinterpret_cast<char*>(data.data.data()), sizeof(TileData) * stage_size);

	file.close();
}

void StageData::FetchData(const Terrain& terrain)
{
	const size_t width{ static_cast<size_t>(terrain.GetMapWidth()) };
	const size_t height{ static_cast<size_t>(terrain.GetMapHeight()) };
	const auto& map{ terrain.GetMap() };

	data.data.resize(width * height);

	// �}�b�v����e�^�C���̃f�[�^�𒊏o���A�񎟔z��̃f�[�^���ꎟ�z��ɕϊ�����B
	for (size_t ri = 0U; ri < height; ++ri)
	{
		for (size_t ci = 0U; ci < width; ++ci)
		{
			data.data[ci + ri * width] = map[ci][ri].data;
		}
	}

}
