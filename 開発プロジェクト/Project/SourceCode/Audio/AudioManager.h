#pragma once

/* --- ヘッダファイルのインクルード ---- */

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

/* --- 宣言 ---- */

// オーディオ
class AudioManager : public Singleton<AudioManager>
{
public:

	AudioManager();
	~AudioManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	void ResetVolume();

	void Update();

	/// <summary>
	/// 終了処理
	/// </summary>
	void EndProcess();

	/// <summary>
	/// 読み込んだオーディオデータを解放する
	/// </summary>
	void ClearResources();

	void ImGuiRender();

	/// <summary>
	/// ファイルパスのオーディオリソースを読み込み、ソースを生成する。
	/// パスのリソースが無ければそれも生成する。
	/// </summary>
	/// <param name="filepath">ファイルパス</param>
	/// <param name="is_loop">オーディオのループフラグ、返り値のポインタを別で保持しないと音を止められないので注意</param>
	const std::shared_ptr<AudioSource>& CreateAudioSource(const wchar_t* filepath, bool is_loop);

	/// <summary>
	/// ファイルパスのオーディオリソースを読み込む
	/// </summary>
	/// <returns>既に読み込んでいた場合はfalse</returns>
	bool LoadAudioResource(const wchar_t* filepath);

	/// <summary>
	/// 引数のディレクトリ以下のwavファイルをすべて読み込む
	/// </summary>
	/// <param name="directory"></param>
	/// <returns></returns>
	void LoadAudioResourcesRecursive(const wchar_t* directory_path);

	/// <summary>
	/// XAudio2インターフェースのポインタ取得
	/// </summary>
	IXAudio2* GetXAudio() const { return xaudio.Get(); }

	/// <summary>
	/// ゲーム全体の音量
	/// </summary>
	float GetMasteringVoiceVolume() const;
	void SetMasteringVoiceVolume(float f);

	/// <summary>
	/// SEの音量
	/// </summary>
	float GetSEVolume() const { return se_volume; }
	void SetSEVolume(float f) { se_volume = f; }

	/// <summary>
	/// BGMの音量
	/// </summary>
	float GetBGMVolume() const { return bgm_volume; }
	void SetBGMVolume(float f) { bgm_volume = f; }

	/// <summary>
	/// オーディオにエコーエフェクトをかける
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="wet_dry_mix">ウェット (処理済み) シグナルとドライ (元の) シグナルの比率</param>
	/// <param name="feed_back">入力にフィードバックする出力の量。</param>
	/// <param name="delay">すべてのチャネルへの遅延 (ミリ秒単位)。この値は 、FXECHO_MIN_DELAY と FXECHO_INITDATAの間である必要があります。MaxDelay。</param>
	void EchoEffect(
		const std::shared_ptr<AudioSource>& dst,
		float wet_dry_mix = FXECHO_DEFAULT_WETDRYMIX,
		float feed_back = FXECHO_DEFAULT_FEEDBACK,
		float delay = FXECHO_DEFAULT_DELAY);

	/// <summary>
	/// オーディオに4バンドイコライザーエフェクトをかける
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="frequency_center0">Hz 単位の中心周波数。 FXEQ_MIN_FREQUENCY_CENTERとFXEQ_MAX_FREQUENCY_CENTERの間である必要があります。</param>
	/// <param name="gain0">周波数に対するブーストまたは減少。 FXEQ_MIN_GAINとFXEQ_MAX_GAINの間である必要があります</param>
	/// <param name="bandwidth0">バンドの幅FXEQ_MIN_BANDWIDTHとFXEQ_MAX_BANDWIDTHの間である必要があります。</param>
	void EqualizerEffect(
		const std::shared_ptr<AudioSource>& dst,
		float frequency_center0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0, float gain0 = FXEQ_DEFAULT_GAIN, float bandwidth0 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1, float gain1 = FXEQ_DEFAULT_GAIN, float bandwidth1 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2, float gain2 = FXEQ_DEFAULT_GAIN, float bandwidth2 = FXEQ_DEFAULT_BANDWIDTH,
		float frequency_center3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3, float gain3 = FXEQ_DEFAULT_GAIN, float bandwidth3 = FXEQ_DEFAULT_BANDWIDTH);

	/// <summary>
	/// オーディオにボリュームリミッターエフェクトをかける
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="release">オーディオがラウ ドネス メンバーによって指定されたリミッターのしきい値を下回った後、リミッターがオーディオへの影響を停止する速度 (ミリ秒単位)。 この値は 、FXMASTERINGLIMITER_MIN_RELEASE (1) ~ FXMASTERINGLIMITER_MAX_RELEASE (20) の間で、既定値 は FXMASTERINGLIMITER_DEFAULT_RELEASE (6) にする必要があります。</param>
	/// <param name="loudness">リミッターのラウドネス メトリックしきい値。 この値は 、FXMASTERINGLIMITER_MIN_LOUDNESS (1) ~ FXMASTERINGLIMITER_MAX_LOUDNESS (1800) の間で指定し、既定値 は FXMASTERINGLIMITER_DEFAULT_LOUDNESS (1000) にする必要があります。</param>
	void MasteringLimiterEffect(
		const std::shared_ptr<AudioSource>& dst,
		uint32_t release = FXMASTERINGLIMITER_DEFAULT_RELEASE,
		uint32_t loudness = FXMASTERINGLIMITER_DEFAULT_LOUDNESS);

	/// <summary>
	/// オーディオにリバーブエフェクトをかける
	/// </summary>
	/// <param name="dst"></param>
	/// <param name="diffusion">個々の壁反射の文字を制御します。 ハード フラット サーフェスをシミュレートする場合は最小値に、拡散サーフェスをシミュレートする場合は最大値に設定します。値は、FXREVERB_MIN_DIFFUSIONとFXREVERB_MAX_DIFFUSIONの間である必要があります。</param>
	/// <param name="room_size">部屋の広さ。 値は、FXREVERB_MIN_ROOMSIZEとFXREVERB_MAX_ROOMSIZEの間である必要があります。 RoomSize の物理的な意味は主観的であり、特定の単位には関連付けられていないことに注意してください。 値を小さくすると、リフレクションがリスナーにすばやく到達しますが、RoomSize の値が大きいほどリフレクションに時間がかかります。</param>
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

	// ソースボイス、サブミックスボイスのデータを受信しオーディオを
	// サウンドデバイスから出力する為の最終出力先。
	IXAudio2MasteringVoice* mastering_voice;

	// 読み込んだオーディオデータ
	std::unordered_map<std::wstring, std::shared_ptr<AudioResource>> audio_resources;
	// 生成したオーディオソース。バッファを消費しきった物は破棄される。
	std::unordered_set<std::shared_ptr<AudioSource>> audio_sources;

	float se_volume;
	float bgm_volume;

};

