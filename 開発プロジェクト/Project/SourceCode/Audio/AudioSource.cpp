#include "AudioSource.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Include/misc.h"
#include "AudioManager.h"

/* --- 定義 ---- */

// コンストラクタ
AudioSource::AudioSource(const std::shared_ptr<AudioResource>& resource, bool is_loop)
{
	Initialize(resource, is_loop);
}

// デストラクタ
AudioSource::~AudioSource()
{
	if (source_voice)
	{
		source_voice->DestroyVoice();
		source_voice = nullptr;
	}
}

void AudioSource::Initialize(const std::shared_ptr<AudioResource>& resource, bool is_loop)
{
	HRESULT hr{ S_OK };

	// ソースボイスを生成
	hr = AudioManager::GetInstance().GetXAudio()->CreateSourceVoice(
		&source_voice,
		&resource->GetWaveFormat(),
		0,
		2.0f,
		nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ソースボイスにデータを送信
	::ZeroMemory(&source_buffer, sizeof(source_buffer));
	source_buffer.AudioBytes = static_cast<UINT32>(resource->GetAudioBytes());
	source_buffer.pAudioData = resource->GetAudioData();
	source_buffer.LoopCount = is_loop ? XAUDIO2_LOOP_INFINITE : 0;
	source_buffer.Flags = XAUDIO2_END_OF_STREAM;

	// Waveフォーマットを確保する
	wave_format = resource->GetWaveFormat();

	source_voice->SubmitSourceBuffer(&source_buffer);
}

void AudioSource::Play(float volume)
{
	HRESULT hr = source_voice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), L"オーディオの再生に失敗しました。");
	source_voice->SetVolume(volume);
}

void AudioSource::Resume()
{
	HRESULT hr = source_voice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), L"オーディオの再生に失敗しました。");
}

// 停止
void AudioSource::Stop()
{
	source_voice->Stop();
}

void AudioSource::SetVolume(float volume)
{
	source_voice->SetVolume(volume);
}

bool AudioSource::IsEnd() const
{
	XAUDIO2_VOICE_STATE state;
	source_voice->GetState(&state);
	// キューは消費したサウンドバッファの数を表す。
	return state.BuffersQueued == 0; 
}
