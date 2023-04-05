#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <xaudio2.h>
#include "AudioResource.h"	

/* --- 宣言 ---- */

class AudioSource
{
public:

	AudioSource() = default;
	AudioSource(const std::shared_ptr<AudioResource>& resource, bool is_loop);
	~AudioSource();

	void Initialize(const std::shared_ptr<AudioResource>& resource, bool is_loop);

	void Play(float volume = 1.0f);

	void Resume();

	void Stop();
	// 音量設定
	void SetVolume(float volume);

	bool IsEnd() const;

	IXAudio2SourceVoice* GetSource() { return source_voice; }

private:

	IXAudio2SourceVoice* source_voice;

	XAUDIO2_BUFFER source_buffer;
	WAVEFORMATEX wave_format;

};