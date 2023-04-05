#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cassert>
#include <vector>
#include <functional>

/* --- 宣言 ---- */

class Combination
{
public:

    /// <summary>
    /// 全ての組み合わせでコールバック関数の処理を行う
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="arr">組み合わせを行う配列</param>
    /// <param name="target_size">いくつを選択する組み合わせか</param>
    /// <param name="callback">trueを返した時点で処理を終了する</param>
    template<class T>
    static void Execute(const std::vector<T>& arr, int target_size, std::function<bool(const std::vector<T>&)> callback) {

        std::vector<int> indices(target_size);
        const int array_size = arr.size();
        int start_index = 0;
        int size = 0;

        while (size >= 0)
        {
            for (int i = start_index; i < array_size; ++i)
            {
                indices[size++] = i;

                if (size == target_size)
                {
                    std::vector<T> comb(target_size);

                    for (int x = 0; x < target_size; ++x)
                    {
                        comb[x] = arr[indices[x]];
                    }
                    if (callback(comb)) return;
                    break;
                }
            }

            --size;
            if (size < 0) break;
            start_index = indices[size] + 1;
        }
    }

    /// <summary>
    /// 全ての組み合わせでコールバック関数の処理を行う
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="arr">組み合わせを行う配列</param>
    /// <param name="target_size">いくつを選択する組み合わせか</param>
    template<class T>
    static void Execute(const std::vector<T>& arr, int target_size, std::function<void(const std::vector<T>&)> callback) {

        std::vector<int> indices(target_size);
        const size_t& array_size = arr.size();
        int start_index = 0;
        int size = 0;

        while (size >= 0)
        {
            for (int i = start_index; i < array_size; ++i)
            {
                indices[size++] = i;

                if (size == target_size)
                {
                    std::vector<T> comb(target_size);

                    for (int x = 0; x < target_size; ++x)
                    {
                        comb[x] = arr[indices[x]];
                    }

                    callback(comb);

                    break;
                }
            }

            --size;
            if (size < 0) break;
            start_index = indices[size] + 1;
        }
    }

    /// <summary>
    /// 組み合わせの計算
    /// https://qiita.com/mosamosa/items/79cf230ae22ee71f6801
    /// </summary>
    /// <param name="n">総数</param>
    /// <param name="r">選ぶ数</param>
    /// <returns></returns>
    static std::vector<std::vector<long long>> Comb(int n, int r) {
        std::vector<std::vector<long long>> v(n + 1, std::vector<long long>(n + 1, 0));
        for (int i = 0; i < v.size(); i++) {
            v[i][0] = 1;
            v[i][i] = 1;
        }
        for (int j = 1; j < v.size(); j++) {
            for (int k = 1; k < j; k++) {
                v[j][k] = (v[j - 1][k - 1] + v[j - 1][k]);
            }
        }
        return v;
    }

    /// <summary>
    /// 組み合わせの計算
    /// https://qiita.com/mosamosa/items/79cf230ae22ee71f6801
    /// </summary>
    /// <param name="n">総数</param>
    /// <param name="r">選ぶ数</param>
    /// <returns>組み合わせの総数</returns>
    static long long Count(int n, int r) 
    {
        return Comb(n, r)[n][r];
    }
};

#if 0
// なんか早いやつ
int Combination(int n, int r)
{
    assert(!(n < 0 || r < 0 || r > n));

    if (n - r < r) r = n - r;
    if (r == 0) return 1;
    if (r == 1) return n;

    int* numerator = new int[r];
    int* denominator = new int[r];

    for (int k = 0; k < r; k++)
    {
        numerator[k] = n - r + k + 1;
        denominator[k] = k + 1;
    }

    for (int p = 2; p <= r; p++)
    {
        int pivot = denominator[p - 1];
        if (pivot > 1)
        {
            int offset = (n - r) % p;
            for (int k = p - 1; k < r; k += p)
            {
                numerator[k - offset] /= pivot;
                denominator[k] /= pivot;
            }
        }
    }

    int result = 1;
    for (int k = 0; k < r; k++)
    {
        if (numerator[k] > 1) result *= numerator[k];
    }

    return result;
}
#endif
