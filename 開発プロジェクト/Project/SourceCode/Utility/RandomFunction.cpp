#include "RandomFunction.h"

/* --- 追加のヘッダファイルのインクルード ---- */

/* --- 定義 ---- */

std::random_device Random::seed_generator;
std::mt19937 Random::engine(Random::seed_generator());
