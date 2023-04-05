#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <vector>
#include <windows.h>

/* --- �錾 ---- */

class AudioResource
{
private:

	// RIFF�w�b�_
	struct Riff
	{
		uint32_t tag;			// RIFF�`���̎��ʎq 'RIFF'
		uint32_t size;			// ����ȍ~�̃t�@�C���T�C�Y(�t�@�C���T�C�Y - 8)
		uint32_t type;			// RIFF�̎�ނ�\�����ʎq 'WAVE'
	};

	// �`�����N
	struct Chunk
	{
		uint32_t tag;			// �`�����N�`���̎��ʎq 'fmt ' 'data'
		uint32_t size;			// �f�[�^�T�C�Y('fmt '���j�APCM�Ȃ��16 'data'�g�`�f�[�^�T�C�Y)
	};

	// fmt �`�����N
	struct Fmt
	{
		uint16_t fmtId;			// �t�H�[�}�b�gID(���j�APCM�Ȃ��1)
		uint16_t channel;		// �`�����l����(���m����:1 �X�e���I:2)
		uint32_t sampleRate;	// �T���v�����O���[�g(44.1kHz�Ȃ�44100)
		uint32_t transRate;		// �f�[�^���x(Byte/sec) [ 44.1kHz 16bit �X�e���I 44100�~2�~2 ]
		uint16_t blockSize;		// �u���b�N�T�C�Y(Byte/sample�~�`�����l����)
		uint16_t quantumBits;	// �T���v��������̃r�b�g��(bit/sample)
	};

public:

	AudioResource(const wchar_t* filepath);
	~AudioResource();
	/// <summary>
	/// �I�[�f�B�I�f�[�^�̎擾
	/// </summary>
	uint8_t* GetAudioData() { return data.data(); }
	/// <summary>
	/// �I�[�f�B�I�̃o�C�g�T�C�Y�̎擾
	/// </summary>
	size_t GetAudioBytes() const { return data.size(); }
	/// <summary>
	/// wav�̃t�H�[�}�b�g�̎擾
	/// </summary>
	const WAVEFORMATEX& GetWaveFormat() const { return wfx; }

private:

	Riff riff;
	Fmt fmt;
	std::vector<uint8_t> data;
	WAVEFORMATEX wfx;
};
