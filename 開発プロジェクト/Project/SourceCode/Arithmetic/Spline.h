#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Arithmetic.h"

/* --- 宣言 ---- */

class Spline
{
public:
	/// <summary>
	/// Catmull-Rom スプライン補間
	/// </summary>
	/// <typeparam name="T">Vector2, Vector3, (Vector4)</typeparam>
	/// <param name="t">補間係数(0~1)</param>
	/// <returns>補間結果</returns>
	template<class T>
	static T CatmullRom(const T& a, const T& b, const T& c, const T& d, float t);


};

template<class T>
inline T Spline::CatmullRom(const T& a, const T& b, const T& c, const T& d, float t)
{
    // https://zenn.dev/mushe/articles/92c65e0c8023aa

    float t2{ t * t };
    float t3{ t2 * t };

    //float ta{ -t3 + 2.0f * t2 - t };
    //float tb{ 3.0f * t3 - 5.0f * t2 + 2.0f };
    //float tc{ -3.0f * t3 + 4.0f * t2 + t };
    //float td{ t3 - t2 };

    //return ((ta * a) + (tb * b) + (tc * c) + (td * d)) * 0.5f;

    return ((-t3 + 2.0f * t2 - t) * a +
        (3.0f * t3 - 5.0f * t2 + 2.0f) * b +
        (-3.0f * t3 + 4.0f * t2 + t) * c +
        (t3 - t2) * d) * 0.5f;
}
