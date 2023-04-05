#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <vector>
#include <windows.h>

/* --- 宣言 ---- */

class AudioResource
{
private:

	// RIFFヘッダ
	struct Riff
	{
		uint32_t tag;			// RIFF形式の識別子 'RIFF'
		uint32_t size;			// これ以降のファイルサイズ(ファイルサイズ - 8)
		uint32_t type;			// RIFFの種類を表す識別子 'WAVE'
	};

	// チャンク
	struct Chunk
	{
		uint32_t tag;			// チャンク形式の識別子 'fmt ' 'data'
		uint32_t size;			// データサイズ('fmt 'リニアPCMならば16 'data'波形データサイズ)
	};

	// fmt チャンク
	struct Fmt
	{
		uint16_t fmtId;			// フォーマットID(リニアPCMならば1)
		uint16_t channel;		// チャンネル数(モノラル:1 ステレオ:2)
		uint32_t sampleRate;	// サンプリングレート(44.1kHzなら44100)
		uint32_t transRate;		// データ速度(Byte/sec) [ 44.1kHz 16bit ステレオ 44100×2×2 ]
		uint16_t blockSize;		// ブロックサイズ(Byte/sample×チャンネル数)
		uint16_t quantumBits;	// サンプルあたりのビット数(bit/sample)
	};

public:

	AudioResource(const wchar_t* filepath);
	~AudioResource();
	/// <summary>
	/// オーディオデータの取得
	/// </summary>
	uint8_t* GetAudioData() { return data.data(); }
	/// <summary>
	/// オーディオのバイトサイズの取得
	/// </summary>
	size_t GetAudioBytes() const { return data.size(); }
	/// <summary>
	/// wavのフォーマットの取得
	/// </summary>
	const WAVEFORMATEX& GetWaveFormat() const { return wfx; }

private:

	Riff riff;
	Fmt fmt;
	std::vector<uint8_t> data;
	WAVEFORMATEX wfx;
};
