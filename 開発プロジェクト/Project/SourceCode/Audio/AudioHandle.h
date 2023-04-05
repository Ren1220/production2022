#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xapo.h>
#include <xapofx.h>

#include "Audio/AudioSource.h"

/* --- 宣言 ---- */

/// <summary>
/// Play関数実行時に管理クラスで生成したAudioSourceを持ち、そのAudioSourceを操作するクラス。
/// 一インスタンスにつき一つのソースボイスを持ち、再生中に再度Playすると既に持っていたソースボイスの参照は出来なくなる。
/// （大元のポインタは管理クラスが持っているため、再生が終了することはない。）
/// </summary>
class AudioHandle
{
public:

	AudioHandle();
	~AudioHandle();

	/// <summary>
	/// 管理クラスでソースボイスを生成し、そのポインタの弱参照を保持する。
	/// 再生中に再度Playすると既に持っていたソースボイスの参照は出来なくなる。
	/// </summary>
	/// <param name="filepath"></param>
	/// <param name="volume"></param>
	/// <param name="is_loop">オーディオのループフラグ、この参照を失うとループ再生を止めることができないので注意</param>
	/// <param name="is_bgm">SEとBGMで処理（主にボリューム設定）が異なる</param>
	void Play(const wchar_t* filepath, float volume = 1.0f, bool is_loop = false, bool is_bgm = false);

	void Stop();

	/// <summary>
	/// オーディオにリバーブエフェクトをかける
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="diffusion">個々の壁反射の文字を制御します。 ハード フラット サーフェスをシミュレートする場合は最小値に、拡散サーフェスをシミュレートする場合は最大値に設定します。値は、FXREVERB_MIN_DIFFUSIONとFXREVERB_MAX_DIFFUSIONの間である必要があります。</param>
	/// <param name="room_size">部屋の広さ。 値は、FXREVERB_MIN_ROOMSIZEとFXREVERB_MAX_ROOMSIZEの間である必要があります。 RoomSize の物理的な意味は主観的であり、特定の単位には関連付けられていないことに注意してください。 値を小さくすると、リフレクションがリスナーにすばやく到達しますが、RoomSize の値が大きいほどリフレクションに時間がかかります。</param>
	void ReverbEffect(
		float diffusion = FXREVERB_DEFAULT_DIFFUSION,
		float room_size = FXREVERB_DEFAULT_ROOMSIZE);

private:

	std::weak_ptr<AudioSource> w_source;
};
