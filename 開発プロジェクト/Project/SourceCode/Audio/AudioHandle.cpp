#include "AudioHandle.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Audio/AudioManager.h"

/* --- 定義 ---- */

AudioHandle::AudioHandle()
{
}

AudioHandle::~AudioHandle()
{
}

void AudioHandle::Play(const wchar_t* filepath, float volume, bool is_loop, bool is_bgm)
{
	const auto& p{ AudioManager::GetInstance().CreateAudioSource(filepath, is_loop) };
	p->Play();
	p->SetVolume(volume * (is_bgm ? AudioManager::GetInstance().GetBGMVolume() : AudioManager::GetInstance().GetSEVolume()));
	w_source = p;
}

void AudioHandle::Stop()
{
	if (auto s = w_source.lock())
	{
		s->Stop();
	}
}

void AudioHandle::ReverbEffect(float diffusion, float room_size)
{
	if (auto s = w_source.lock())
	{
		AudioManager::GetInstance().ReverbEffect(s, diffusion, room_size);
	}
}
