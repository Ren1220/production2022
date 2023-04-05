#include "AudioManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <algorithm>
#include <filesystem>

#include "imgui.h"
#include "General/MyImGuiHelper.h"
#include "Include/misc.h"

/* --- 定義 ---- */

AudioManager::AudioManager()
	:audio_resources(), audio_sources(), se_volume(1.0f), bgm_volume(1.0f)
{
}

AudioManager::~AudioManager()
{
}

void AudioManager::Initialize()
{
	HRESULT hr;

	// COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(xaudio.ReleaseAndGetAddressOf(), createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// マスタリングボイス生成
	hr = xaudio->CreateMasteringVoice(&mastering_voice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	mastering_voice->SetVolume(1.0f);
}

void AudioManager::ResetVolume()
{
}

void AudioManager::Update()
{
	// 再生を終えたオーディオソースの破棄
	{
		std::list<std::shared_ptr<AudioSource>> is_end_audio;
		for (const auto& s : audio_sources)
		{
			if (s->IsEnd())
			{
				is_end_audio.emplace_back(s);
			}
		}

		for (const auto& e : is_end_audio)
		{
			auto itr{ audio_sources.find(e) };
			if (itr != audio_sources.end())
			{
				// デストラクタでXAudioSourceVoiceの破棄がされる
				audio_sources.erase(itr);
			}
		}
	}
}

static std::weak_ptr<AudioSource> playing_audio{ };

void AudioManager::EndProcess()
{
	if (auto audio = playing_audio.lock())
	{
		playing_audio.reset();
	}

	ClearResources();

	if (mastering_voice)
	{
		mastering_voice->DestroyVoice();
		mastering_voice = nullptr;
	}

	//// COM終了化
	CoUninitialize();
}

void AudioManager::ClearResources()
{
	// 先にソースボイス
	audio_sources.clear();
	audio_resources.clear();
}

void AudioManager::ImGuiRender()
{
	auto func = [](void* data, int n, const char** out_text) ->bool
	{
		auto resources{ reinterpret_cast<std::unordered_map<std::string, std::shared_ptr<AudioResource>>*>(data) };

		if (resources->size() <= static_cast<size_t>(n)) return false;
		auto itr{ std::next(resources->begin(), n) };
		*out_text = itr->first.c_str();
		return true;
	};

	static int itrpos{ 0 };
	ImGui::BeginListBox("AudioList", { 400, 400 });

	for (int i = 0; const auto& a : audio_resources)
	{
		// utf-8に変換
		char utf8str[100]{};
		WideCharToMultiByte(CP_UTF8, 0, a.first.c_str(), -1, utf8str, 100, nullptr, nullptr);

		if (ImGui::Selectable(utf8str, itrpos == i)) itrpos = i;
		++i;
	}
	ImGui::EndListBox();


	static int mode = { 0 };
	if (ImGui::RadioButton("Echo", mode == 0)) { mode == 0 ? mode = -1 : mode = 0; } ImGui::SameLine();
	if (ImGui::RadioButton("Equalizer", mode == 1)) { mode == 1 ? mode = -1 : mode = 1; } ImGui::SameLine();
	if (ImGui::RadioButton("MasetringLimiter", mode == 2)) { mode == 2 ? mode = -1 : mode = 2; }ImGui::SameLine();
	if (ImGui::RadioButton("Reverb", mode == 3)) { mode == 3 ? mode = -1 : mode = 3; }

	switch (mode)
	{
		case 0: ImGuiEcho(itrpos); break;
		case 1: ImGuiEqualizer(itrpos); break;
		case 2: ImGuiMasteringLimiter(itrpos); break;
		case 3: ImGuiReverb(itrpos); break;


	default:
		if (ImGui::Button("Play"))
		{
			auto itr{ std::next(audio_resources.begin(), itrpos) };

			playing_audio = CreateAudioSource(itr->first.c_str(), false);
			playing_audio.lock()->Play();
		}
		break;
	}
}

const std::shared_ptr<AudioSource>& AudioManager::CreateAudioSource(const wchar_t* filepath, bool is_loop)
{
	// リソースを読み込む
	LoadAudioResource(filepath);

	// 生成したデータを挿入し、そのデータのポインタの参照を返す
	return *audio_sources.emplace(std::make_shared<AudioSource>(audio_resources[filepath], is_loop)).first;
}

bool AudioManager::LoadAudioResource(const wchar_t* filepath)
{
	// キャッシュ内に同一パスのオーディオデータが存在するか検索する
	auto itr{ audio_resources.find(filepath) };

	// データが存在する場合
	if (itr != audio_resources.end()) return false;

	// リソースを生成
	audio_resources[filepath] = std::make_shared<AudioResource>(filepath);

	return true;
}

void AudioManager::LoadAudioResourcesRecursive(const wchar_t* directory_path)
{
	namespace fs = std::filesystem;

	_ASSERT_EXPR(fs::is_directory(directory_path), L"引数のディレクトリが存在しません");

	const auto h_extension{ std::hash<fs::path>()(L".wav") };

	for (const auto& e : fs::recursive_directory_iterator(directory_path))
	{
		// パスの拡張子が.wavの場合
		if (h_extension == std::hash<fs::path>()(e.path().extension()))
		{
			// オーディオリソースを生成する
			LoadAudioResource(e.path().wstring().c_str());
		}
	}

}

float AudioManager::GetMasteringVoiceVolume() const
{
	float v;
	mastering_voice->GetVolume(&v);
	return v;
}

void AudioManager::SetMasteringVoiceVolume(float f)
{
	mastering_voice->SetVolume(f);
}

void AudioManager::EchoEffect(
	const std::shared_ptr<AudioSource>& dst,
	float wet_dry_mix, 
	float feed_back, 
	float delay)
{
	HRESULT hr{ S_OK };

	if (dst == nullptr) return;

	IUnknown* xapo;
	hr = ::CreateFX(__uuidof(::FXEcho), &xapo);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (auto source_voice = dst->GetSource())
	{
		SetEffectChain(source_voice, xapo);

		FXECHO_PARAMETERS param
		{
			wet_dry_mix, feed_back, delay
		};

		hr = source_voice->SetEffectParameters(
			0,
			&param,
			sizeof(param));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void AudioManager::EqualizerEffect(
	const std::shared_ptr<AudioSource>& dst,
	float frequency_center0, float gain0, float bandwidth0, 
	float frequency_center1, float gain1, float bandwidth1, 
	float frequency_center2, float gain2, float bandwidth2, 
	float frequency_center3, float gain3, float bandwidth3)
{
	HRESULT hr{ S_OK };

	if (dst == nullptr) return;

	IUnknown* xapo;
	hr = ::CreateFX(__uuidof(::FXEQ), &xapo);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (auto source_voice = dst->GetSource())
	{
		SetEffectChain(source_voice, xapo);

		FXEQ_PARAMETERS param
		{
			frequency_center0, gain0, bandwidth0,
			frequency_center1, gain1, bandwidth1,
			frequency_center2, gain2, bandwidth2,
			frequency_center3, gain3, bandwidth3
		};

		hr = source_voice->SetEffectParameters(
			0,
			&param,
			sizeof(param));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void AudioManager::MasteringLimiterEffect(
	const std::shared_ptr<AudioSource>& dst,
	uint32_t release, 
	uint32_t loudness)
{
	HRESULT hr{ S_OK };

	if (dst == nullptr) return;

	IUnknown* xapo;
	hr = ::CreateFX(__uuidof(::FXMasteringLimiter), &xapo);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (auto source_voice = dst->GetSource())
	{
		SetEffectChain(source_voice, xapo);

		FXMASTERINGLIMITER_PARAMETERS param
		{
			release, loudness
		};

		hr = source_voice->SetEffectParameters(
			0,
			&param,
			sizeof(param));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void AudioManager::ReverbEffect(
	const std::shared_ptr<AudioSource>& dst,
	float diffusion,
	float room_size)
{
	HRESULT hr{ S_OK };

	if (dst == nullptr) return;

	IUnknown* xapo;
	hr = ::CreateFX(__uuidof(::FXReverb), &xapo);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (auto source_voice = dst->GetSource())
	{
		SetEffectChain(source_voice, xapo);

		FXREVERB_PARAMETERS param
		{
			diffusion, room_size
		};

		hr = source_voice->SetEffectParameters(
			0,
			&param,
			sizeof(param));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void AudioManager::SetEffectChain(
	IXAudio2SourceVoice* source_voice,
	IUnknown* xapo)
{
	HRESULT hr{ S_OK };

	XAUDIO2_EFFECT_DESCRIPTOR descriptor{};
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = xapo;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;
	
	hr = source_voice->SetEffectChain(&chain);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::ImGuiEcho(int itrpos)
{
	if (ImGui::CollapsingHeader("Echo", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static float wet_dry_mix{ FXECHO_DEFAULT_WETDRYMIX };
		static float feed_back{ FXECHO_DEFAULT_FEEDBACK };
		static float delay{ FXECHO_DEFAULT_DELAY };

		ImGui::SliderFloat("WetDryMix", &wet_dry_mix, FXECHO_MIN_WETDRYMIX, FXECHO_MAX_WETDRYMIX);
		ImGui::SliderFloat("FeedBack", &feed_back, FXECHO_MIN_FEEDBACK, FXECHO_MAX_FEEDBACK);
		ImGui::SliderFloat("Delay(ms)", &delay, FXECHO_MIN_DELAY, FXECHO_MAX_DELAY, "%.0f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Play"))
		{
			auto itr{ std::next(audio_resources.begin(), itrpos) };
			playing_audio = CreateAudioSource(itr->first.c_str(), false);
			playing_audio.lock()->Play();
			AudioManager::GetInstance().EchoEffect(playing_audio.lock(), wet_dry_mix, feed_back, delay);
		}
	}
}

void AudioManager::ImGuiEqualizer(int itrpos)
{
	if (ImGui::CollapsingHeader("Equalizer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static float frequency_center[4]{ FXEQ_DEFAULT_FREQUENCY_CENTER_0, FXEQ_DEFAULT_FREQUENCY_CENTER_1, FXEQ_DEFAULT_FREQUENCY_CENTER_2, FXEQ_DEFAULT_FREQUENCY_CENTER_3 };
		static float gain[4]		    { FXEQ_DEFAULT_GAIN,FXEQ_DEFAULT_GAIN,FXEQ_DEFAULT_GAIN,FXEQ_DEFAULT_GAIN };
		static float bandwidth[4]	    { FXEQ_DEFAULT_BANDWIDTH,FXEQ_DEFAULT_BANDWIDTH,FXEQ_DEFAULT_BANDWIDTH,FXEQ_DEFAULT_BANDWIDTH };

		auto band_min = [&](int i) -> float
		{
			return (std::max)(FXEQ_MIN_BANDWIDTH, frequency_center[i] - (bandwidth[i] / 2));
		};

		auto band_max = [&](int i) -> float
		{
			return (std::min)(FXEQ_MAX_BANDWIDTH, frequency_center[i] + (bandwidth[i] / 2));
		};

		ImGui::SliderFloat("FrequencyCenter0", &frequency_center[0], FXEQ_MIN_FREQUENCY_CENTER, FXEQ_MAX_FREQUENCY_CENTER, "%.0f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Gain0", &gain[0], FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.3f");
		ImGui::SliderFloat("Bandwidth0", &bandwidth[0], band_min(0), band_max(0));
		ImGui::SliderFloat("FrequencyCenter1", &frequency_center[1], FXEQ_MIN_FREQUENCY_CENTER, FXEQ_MAX_FREQUENCY_CENTER, "%.0f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Gain1", &gain[1], FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.3f");
		ImGui::SliderFloat("Bandwidth1", &bandwidth[1], band_min(1), band_max(1));
		ImGui::SliderFloat("FrequencyCenter2", &frequency_center[2], FXEQ_MIN_FREQUENCY_CENTER, FXEQ_MAX_FREQUENCY_CENTER, "%.0f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Gain2", &gain[2], FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.3f");
		ImGui::SliderFloat("Bandwidth2", &bandwidth[2], band_min(2), band_max(2));
		ImGui::SliderFloat("FrequencyCenter3", &frequency_center[3], FXEQ_MIN_FREQUENCY_CENTER, FXEQ_MAX_FREQUENCY_CENTER, "%.0f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Gain3", &gain[3], FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.3f");
		ImGui::SliderFloat("Bandwidth3", &bandwidth[3], band_min(3), band_max(3));

		if (ImGui::Button("Play"))
		{
			auto itr{ std::next(audio_resources.begin(), itrpos) };
			playing_audio = CreateAudioSource(itr->first.c_str(), false);
			playing_audio.lock()->Play();
			AudioManager::GetInstance().EqualizerEffect(playing_audio.lock(),
				frequency_center[0], gain[0], bandwidth[0],
				frequency_center[1], gain[1], bandwidth[1],
				frequency_center[2], gain[2], bandwidth[2],
				frequency_center[3], gain[3], bandwidth[3]);
		}
	}
}

void AudioManager::ImGuiMasteringLimiter(int itrpos)
{
	if (ImGui::CollapsingHeader("MasteringLimiter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static int32_t release{ FXMASTERINGLIMITER_DEFAULT_RELEASE };
		static int32_t loudness{ FXMASTERINGLIMITER_DEFAULT_LOUDNESS };

		ImGui::SliderInt("Release", &release, FXMASTERINGLIMITER_MIN_RELEASE, FXMASTERINGLIMITER_MAX_RELEASE);
		ImGui::SliderInt("Loudness", &loudness, FXMASTERINGLIMITER_MIN_LOUDNESS, FXMASTERINGLIMITER_MAX_LOUDNESS);

		if (ImGui::Button("Play"))
		{
			auto itr{ std::next(audio_resources.begin(), itrpos) };
			playing_audio = CreateAudioSource(itr->first.c_str(), false);
			playing_audio.lock()->Play();
			AudioManager::GetInstance().MasteringLimiterEffect(
				playing_audio.lock(),
				release,
				loudness);
		}
	}
}

void AudioManager::ImGuiReverb(int itrpos)
{
	if (ImGui::CollapsingHeader("Reverb", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static float diffusion{ FXREVERB_DEFAULT_DIFFUSION };
		static float room_size{ FXREVERB_DEFAULT_ROOMSIZE };

		ImGui::SliderFloat("Diffusion", &diffusion, FXREVERB_MIN_DIFFUSION, FXREVERB_MAX_DIFFUSION);
		ImGui::SliderFloat("RoomSize", &room_size, FXREVERB_MIN_ROOMSIZE, FXREVERB_MAX_ROOMSIZE, "%.4f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Play"))
		{
			auto itr{ std::next(audio_resources.begin(), itrpos) };
			playing_audio = CreateAudioSource(itr->first.c_str(), false);
			playing_audio.lock()->Play();
			AudioManager::GetInstance().ReverbEffect(
				playing_audio.lock(),
				diffusion,
				room_size);
		}
	}
}
