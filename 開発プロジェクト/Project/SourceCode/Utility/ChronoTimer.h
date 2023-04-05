#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <chrono>
#include <string>
#include <format>
#include <Windows.h>

/* --- 宣言 ---- */

class ChronoTimer
{
public:

	void Start()
	{
		tick = std::chrono::system_clock::now();
	}

	/// <summary>
	/// 時間計測終了、秒(ミリ秒)で計測結果を返す。
	/// <para>[Debug]計測時間を出力する</para>
	/// </summary>
	/// <param name="get_sec"></param>
	/// <param name="measure_name">出力するときの出力名</param>
	/// <returns></returns>
	float Stop(bool get_sec = false, std::wstring measure_name = {}, bool is_output_log = true)
	{
		// 計測終了
		auto duration{ std::chrono::system_clock::now() - tick };
		auto measure = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		float measuref = static_cast<float>(measure);
		if (get_sec)
		{
			measuref /= 1000.0f;
		}

#ifdef _DEBUG
		if (is_output_log)
		{
			// 計測結果を出力ウィンドウに表示する
			std::wstring debug_string;
			debug_string = std::format(L"{} : {}{}\n", measure_name.empty() ? L"計測結果" : measure_name, measuref, get_sec ? L"sec" : L"ms");
			OutputDebugStringW(debug_string.c_str());
		}
#endif // _DEBUG

		return measuref;
	}

	class HighPrecision
	{
	public:
		void Start()
		{
			tick = std::chrono::system_clock::now();
		}

		/// <summary>
		/// 計測結果をマイクロ秒で出力する。
		/// </summary>
		int64_t Stop(std::wstring measure_name = {}, bool is_output_log = true)
		{
			auto d{ std::chrono::system_clock::now() - tick };
			auto dll{ std::chrono::duration_cast<std::chrono::microseconds>(d).count() };

#ifdef _DEBUG
			if (is_output_log)
			{
				// 計測結果を出力ウィンドウに表示する
				std::wstring debug_string;
				debug_string = std::format(L"{} : {}{}\n", measure_name.empty() ? L"計測結果" : measure_name, dll, L"usec");
				OutputDebugStringW(debug_string.c_str());
			}
#endif // _DEBUG

			return dll;
		}
	private:

		std::chrono::system_clock::time_point tick;
	};

private:

	// 計測時間
	std::chrono::system_clock::time_point tick;
};
