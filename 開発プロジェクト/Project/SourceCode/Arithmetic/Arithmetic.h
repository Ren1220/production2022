#pragma once

/* --- 概要 ---- */

// DirectXMathを利用しない自作算術ライブラリ。
// DirectXMathを利用した場合書き込むコード量がとても多く、あまり好きでは無かったため自作。
// SIMDを利用して計算速度の向上を今後検討

/* --- ヘッダファイルのインクルード ---- */

#include <cmath>
#include <format>

#include "Arithmetic\Matrix.h"
#include "Arithmetic\Quaternion.h"
#include "Arithmetic\Vector.h"
