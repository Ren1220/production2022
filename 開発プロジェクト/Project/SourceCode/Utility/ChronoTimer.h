#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <chrono>
#include <string>
#include <format>
#include <Windows.h>

/* --- �錾 ---- */

class ChronoTimer
{
public:

	void Start()
	{
		tick = std::chrono::system_clock::now();
	}

	/// <summary>
	/// ���Ԍv���I���A�b(�~���b)�Ōv�����ʂ�Ԃ��B
	/// <para>[Debug]�v�����Ԃ��o�͂���</para>
	/// </summary>
	/// <param name="get_sec"></param>
	/// <param name="measure_name">�o�͂���Ƃ��̏o�͖�</param>
	/// <returns></returns>
	float Stop(bool get_sec = false, std::wstring measure_name = {}, bool is_output_log = true)
	{
		// �v���I��
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
			// �v�����ʂ��o�̓E�B���h�E�ɕ\������
			std::wstring debug_string;
			debug_string = std::format(L"{} : {}{}\n", measure_name.empty() ? L"�v������" : measure_name, measuref, get_sec ? L"sec" : L"ms");
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
		/// �v�����ʂ��}�C�N���b�ŏo�͂���B
		/// </summary>
		int64_t Stop(std::wstring measure_name = {}, bool is_output_log = true)
		{
			auto d{ std::chrono::system_clock::now() - tick };
			auto dll{ std::chrono::duration_cast<std::chrono::microseconds>(d).count() };

#ifdef _DEBUG
			if (is_output_log)
			{
				// �v�����ʂ��o�̓E�B���h�E�ɕ\������
				std::wstring debug_string;
				debug_string = std::format(L"{} : {}{}\n", measure_name.empty() ? L"�v������" : measure_name, dll, L"usec");
				OutputDebugStringW(debug_string.c_str());
			}
#endif // _DEBUG

			return dll;
		}
	private:

		std::chrono::system_clock::time_point tick;
	};

private:

	// �v������
	std::chrono::system_clock::time_point tick;
};
