#include "AudioSource.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Include/misc.h"
#include "AudioManager.h"

/* --- ��` ---- */

// �R���X�g���N�^
AudioSource::AudioSource(const std::shared_ptr<AudioResource>& resource, bool is_loop)
{
	Initialize(resource, is_loop);
}

// �f�X�g���N�^
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

	// �\�[�X�{�C�X�𐶐�
	hr = AudioManager::GetInstance().GetXAudio()->CreateSourceVoice(
		&source_voice,
		&resource->GetWaveFormat(),
		0,
		2.0f,
		nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �\�[�X�{�C�X�Ƀf�[�^�𑗐M
	::ZeroMemory(&source_buffer, sizeof(source_buffer));
	source_buffer.AudioBytes = static_cast<UINT32>(resource->GetAudioBytes());
	source_buffer.pAudioData = resource->GetAudioData();
	source_buffer.LoopCount = is_loop ? XAUDIO2_LOOP_INFINITE : 0;
	source_buffer.Flags = XAUDIO2_END_OF_STREAM;

	// Wave�t�H�[�}�b�g���m�ۂ���
	wave_format = resource->GetWaveFormat();

	source_voice->SubmitSourceBuffer(&source_buffer);
}

void AudioSource::Play(float volume)
{
	HRESULT hr = source_voice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), L"�I�[�f�B�I�̍Đ��Ɏ��s���܂����B");
	source_voice->SetVolume(volume);
}

void AudioSource::Resume()
{
	HRESULT hr = source_voice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), L"�I�[�f�B�I�̍Đ��Ɏ��s���܂����B");
}

// ��~
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
	// �L���[�͏�����T�E���h�o�b�t�@�̐���\���B
	return state.BuffersQueued == 0; 
}
