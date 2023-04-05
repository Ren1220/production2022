#pragma once


/* --- ヘッダファイルのインクルード ---- */

#include <random>

/* --- 関数の定義 ---- */

class Random
{
public:
	template <class T>
	static T GetValue(T min = (std::numeric_limits<T>::min)(), T max = (std::numeric_limits<T>::max)())
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(engine);
	}

	template <>
	static float GetValue<float>(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(engine);
	}


	template <>
	static double GetValue<double>(double min, double max)
	{
		std::uniform_real_distribution<double> dist(min, max);
		return dist(engine);
	}

private:

	static std::random_device seed_generator;
	static std::mt19937 engine;
};
