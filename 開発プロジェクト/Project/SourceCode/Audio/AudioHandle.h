#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xapo.h>
#include <xapofx.h>

#include "Audio/AudioSource.h"

/* --- �錾 ---- */

/// <summary>
/// Play�֐����s���ɊǗ��N���X�Ő�������AudioSource�������A����AudioSource�𑀍삷��N���X�B
/// ��C���X�^���X�ɂ���̃\�[�X�{�C�X�������A�Đ����ɍēxPlay����Ɗ��Ɏ����Ă����\�[�X�{�C�X�̎Q�Ƃ͏o���Ȃ��Ȃ�B
/// �i�匳�̃|�C���^�͊Ǘ��N���X�������Ă��邽�߁A�Đ����I�����邱�Ƃ͂Ȃ��B�j
/// </summary>
class AudioHandle
{
public:

	AudioHandle();
	~AudioHandle();

	/// <summary>
	/// �Ǘ��N���X�Ń\�[�X�{�C�X�𐶐����A���̃|�C���^�̎�Q�Ƃ�ێ�����B
	/// �Đ����ɍēxPlay����Ɗ��Ɏ����Ă����\�[�X�{�C�X�̎Q�Ƃ͏o���Ȃ��Ȃ�B
	/// </summary>
	/// <param name="filepath"></param>
	/// <param name="volume"></param>
	/// <param name="is_loop">�I�[�f�B�I�̃��[�v�t���O�A���̎Q�Ƃ������ƃ��[�v�Đ����~�߂邱�Ƃ��ł��Ȃ��̂Œ���</param>
	/// <param name="is_bgm">SE��BGM�ŏ����i��Ƀ{�����[���ݒ�j���قȂ�</param>
	void Play(const wchar_t* filepath, float volume = 1.0f, bool is_loop = false, bool is_bgm = false);

	void Stop();

	/// <summary>
	/// �I�[�f�B�I�Ƀ��o�[�u�G�t�F�N�g��������
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="diffusion">�X�̕ǔ��˂̕����𐧌䂵�܂��B �n�[�h �t���b�g �T�[�t�F�X���V�~�����[�g����ꍇ�͍ŏ��l�ɁA�g�U�T�[�t�F�X���V�~�����[�g����ꍇ�͍ő�l�ɐݒ肵�܂��B�l�́AFXREVERB_MIN_DIFFUSION��FXREVERB_MAX_DIFFUSION�̊Ԃł���K�v������܂��B</param>
	/// <param name="room_size">�����̍L���B �l�́AFXREVERB_MIN_ROOMSIZE��FXREVERB_MAX_ROOMSIZE�̊Ԃł���K�v������܂��B RoomSize �̕����I�ȈӖ��͎�ϓI�ł���A����̒P�ʂɂ͊֘A�t�����Ă��Ȃ����Ƃɒ��ӂ��Ă��������B �l������������ƁA���t���N�V���������X�i�[�ɂ��΂₭���B���܂����ARoomSize �̒l���傫���قǃ��t���N�V�����Ɏ��Ԃ�������܂��B</param>
	void ReverbEffect(
		float diffusion = FXREVERB_DEFAULT_DIFFUSION,
		float room_size = FXREVERB_DEFAULT_ROOMSIZE);

private:

	std::weak_ptr<AudioSource> w_source;
};
