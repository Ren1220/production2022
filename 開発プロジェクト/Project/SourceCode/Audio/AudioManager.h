#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <wrl.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xapo.h>
#include <xapofx.h>

#include "Utility/Singleton.h"
#include "AudioSource.h"

/* --- �錾 ---- */

// �I�[�f�B�I
class AudioManager : public Singleton<AudioManager>
{
public:

	AudioManager();
	~AudioManager();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

	void ResetVolume();

	void Update();

	/// <summary>
	/// �I������
	/// </summary>
	void EndProcess();

	/// <summary>
	/// �ǂݍ��񂾃I�[�f�B�I�f�[�^���������
	/// </summary>
	void ClearResources();

	void ImGuiRender();

	/// <summary>
	/// �t�@�C���p�X�̃I�[�f�B�I���\�[�X��ǂݍ��݁A�\�[�X�𐶐�����B
	/// �p�X�̃��\�[�X��������΂������������B
	/// </summary>
	/// <param name="filepath">�t�@�C���p�X</param>
	/// <param name="is_loop">�I�[�f�B�I�̃��[�v�t���O�A�Ԃ�l�̃|�C���^��ʂŕێ����Ȃ��Ɖ����~�߂��Ȃ��̂Œ���</param>
	const std::shared_ptr<AudioSource>& CreateAudioSource(const wchar_t* filepath, bool is_loop);

	/// <summary>
	/// �t�@�C���p�X�̃I�[�f�B�I���\�[�X��ǂݍ���
	/// </summary>
	/// <returns>���ɓǂݍ���ł����ꍇ��false</returns>
	bool LoadAudioResource(const wchar_t* filepath);

	/// <summary>
	/// �����̃f�B���N�g���ȉ���wav�t�@�C�������ׂēǂݍ���
	/// </summary>
	/// <param name="directory"></param>
	/// <returns></returns>
	void LoadAudioResourcesRecursive(const wchar_t* directory_path);

	/// <summary>
	/// XAudio2�C���^�[�t�F�[�X�̃|�C���^�擾
	/// </summary>
	IXAudio2* GetXAudio() const { return xaudio.Get(); }

	/// <summary>
	/// �Q�[���S�̂̉���
	/// </summary>
	float GetMasteringVoiceVolume() const;
	void SetMasteringVoiceVolume(float f);

	/// <summary>
	/// SE�̉���
	/// </summary>
	float GetSEVolume() const { return se_volume; }
	void SetSEVolume(float f) { se_volume = f; }

	/// <summary>
	/// BGM�̉���
	/// </summary>
	float GetBGMVolume() const { return bgm_volume; }
	void SetBGMVolume(float f) { bgm_volume = f; }

	/// <summary>
	/// �I�[�f�B�I�ɃG�R�[�G�t�F�N�g��������
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="wet_dry_mix">�E�F�b�g (�����ς�) �V�O�i���ƃh���C (����) �V�O�i���̔䗦</param>
	/// <param name="feed_back">���͂Ƀt�B�[�h�o�b�N����o�̗͂ʁB</param>
	/// <param name="delay">���ׂẴ`���l���ւ̒x�� (�~���b�P��)�B���̒l�� �AFXECHO_MIN_DELAY �� FXECHO_INITDATA�̊Ԃł���K�v������܂��BMaxDelay�B</param>
	void EchoEffect(
		const std::shared_ptr<AudioSource>& dst,
		float wet_dry_mix = FXECHO_DEFAULT_WETDRYMIX,
		float feed_back = FXECHO_DEFAULT_FEEDBACK,
		float delay = FXECHO_DEFAULT_DELAY);

	/// <summary>
	/// �I�[�f�B�I��4�o���h�C�R���C�U�[�G�t�F�N�g��������
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="frequency_center0">Hz �P�ʂ̒��S���g���B FXEQ_MIN_FREQUENCY_CENTER��FXEQ_MAX_FREQUENCY_CENTER�̊Ԃł���K�v������܂��B</param>
	/// <param name="gain0">���g���ɑ΂���u�[�X�g�܂��͌����B FXEQ_MIN_GAIN��FXEQ_MAX_GAIN�̊Ԃł���K�v������܂�</param>
	/// <param name="bandwidth0">�o���h�̕�FXEQ_MIN_BANDWIDTH��FXEQ_MAX_BANDWIDTH�̊Ԃł���K�v������܂��B</param>
	void EqualizerEffect(
		const std::shared_ptr<AudioSource>& dst,
		float frequency_center0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0, float gain0 = FXEQ_DEFAULT_GAIN, float bandwidth0 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1, float gain1 = FXEQ_DEFAULT_GAIN, float bandwidth1 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2, float gain2 = FXEQ_DEFAULT_GAIN, float bandwidth2 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3, float gain3 = FXEQ_DEFAULT_GAIN, float bandwidth3 = FXEQ_DEFAULT_BANDWIDTH);

	/// <summary>
	/// �I�[�f�B�I�Ƀ{�����[�����~�b�^�[�G�t�F�N�g��������
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="release">�I�[�f�B�I�����E �h�l�X �����o�[�ɂ���Ďw�肳�ꂽ���~�b�^�[�̂������l�����������A���~�b�^�[���I�[�f�B�I�ւ̉e�����~���鑬�x (�~���b�P��)�B ���̒l�� �AFXMASTERINGLIMITER_MIN_RELEASE (1) ~ FXMASTERINGLIMITER_MAX_RELEASE (20) �̊ԂŁA����l �� FXMASTERINGLIMITER_DEFAULT_RELEASE (6) �ɂ���K�v������܂��B</param>
	/// <param name="loudness">���~�b�^�[�̃��E�h�l�X ���g���b�N�������l�B ���̒l�� �AFXMASTERINGLIMITER_MIN_LOUDNESS (1) ~ FXMASTERINGLIMITER_MAX_LOUDNESS (1800) �̊ԂŎw�肵�A����l �� FXMASTERINGLIMITER_DEFAULT_LOUDNESS (1000) �ɂ���K�v������܂��B</param>
	void MasteringLimiterEffect(
		const std::shared_ptr<AudioSource>& dst,
		uint32_t release = FXMASTERINGLIMITER_DEFAULT_RELEASE,
		uint32_t loudness = FXMASTERINGLIMITER_DEFAULT_LOUDNESS);

	/// <summary>
	/// �I�[�f�B�I�Ƀ��o�[�u�G�t�F�N�g��������
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="diffusion">�X�̕ǔ��˂̕����𐧌䂵�܂��B �n�[�h �t���b�g �T�[�t�F�X���V�~�����[�g����ꍇ�͍ŏ��l�ɁA�g�U�T�[�t�F�X���V�~�����[�g����ꍇ�͍ő�l�ɐݒ肵�܂��B�l�́AFXREVERB_MIN_DIFFUSION��FXREVERB_MAX_DIFFUSION�̊Ԃł���K�v������܂��B</param>
	/// <param name="room_size">�����̍L���B �l�́AFXREVERB_MIN_ROOMSIZE��FXREVERB_MAX_ROOMSIZE�̊Ԃł���K�v������܂��B RoomSize �̕����I�ȈӖ��͎�ϓI�ł���A����̒P�ʂɂ͊֘A�t�����Ă��Ȃ����Ƃɒ��ӂ��Ă��������B �l������������ƁA���t���N�V���������X�i�[�ɂ��΂₭���B���܂����ARoomSize �̒l���傫���قǃ��t���N�V�����Ɏ��Ԃ�������܂��B</param>
	void ReverbEffect(
		const std::shared_ptr<AudioSource>& dst,
		float diffusion = FXREVERB_DEFAULT_DIFFUSION,
		float room_size = FXREVERB_DEFAULT_ROOMSIZE);

private:

	void SetEffectChain(
		IXAudio2SourceVoice* source_voice,
		IUnknown* xapo);

	void ImGuiEcho(int itrpos);
	void ImGuiEqualizer(int itrpos);
	void ImGuiMasteringLimiter(int itrpos);
	void ImGuiReverb(int itrpos);

	Microsoft::WRL::ComPtr<IXAudio2> xaudio;

	// �\�[�X�{�C�X�A�T�u�~�b�N�X�{�C�X�̃f�[�^����M���I�[�f�B�I��
	// �T�E���h�f�o�C�X����o�͂���ׂ̍ŏI�o�͐�B
	IXAudio2MasteringVoice* mastering_voice;

	// �ǂݍ��񂾃I�[�f�B�I�f�[�^
	std::unordered_map<std::wstring, std::shared_ptr<AudioResource>> audio_resources;
	// ���������I�[�f�B�I�\�[�X�B�o�b�t�@��������������͔j�������B
	std::unordered_set<std::shared_ptr<AudioSource>> audio_sources;

	float se_volume;
	float bgm_volume;

};

