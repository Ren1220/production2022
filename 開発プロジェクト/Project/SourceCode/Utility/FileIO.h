#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <fstream>
#include <filesystem>
#include <vector>
#include <span>

/* --- 宣言 ---- */

class FileIO
{
private:
	FileIO()  = default;
	~FileIO() = default;
public:
	FileIO(const FileIO&)         = delete;
	void operator=(const FileIO&) = delete;

public:

	/// <summary>
	/// バイナリファイルで出力する
	/// </summary>
	/// <typeparam name="T">出力するデータの型(自分で作った構造体もいける)</typeparam>
	/// <param name="filepath">(file_path) + ."bin"して出力する</param>
	/// <param name="data">出力するデータ</param>
	template <class T>
	static void ExportAry(const char* filepath, const std::vector<T>& data)
	{
		std::ofstream ofs(std::string(filepath) + ".bin", std::ios::binary | std::ios::out);
		// const char*にキャストしてデータを出力する
		ofs.write(reinterpret_cast<const char*>(data.data()), sizeof(T) * data.size());
		ofs.close();
	}

	/// <summary>
	/// バイナリファイルで出力する
	/// </summary>
	/// <typeparam name="T">出力するデータの型(自分で作った構造体もいける)</typeparam>
	/// <param name="filepath">(file_path) + ."bin"して出力する</param>
	/// <param name="data">出力するデータ</param>
	template <class T>
	static void Export(const char* filepath, const T& data)
	{
		std::ofstream ofs(std::string(filepath) + ".bin", std::ios::binary | std::ios::out);
		// const char*にキャストしてデータを出力する
		ofs.write(reinterpret_cast<const char*>(&data), sizeof(T));
		ofs.close();
	}

	/// <summary>
	/// バイナリファイルを入力する
	/// </summary>
	static std::vector<char> ImportAry(const char* filepath)
	{
		std::vector<char> output;

		// ファイルをateで開く(ストリームを最後にシーク)
		std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
		// 最後の位置（データサイズ）に配列をリサイズ
		output.resize(ifs.tellg());
		// ストリームの位置を最初に戻す
		ifs.seekg(0, std::ios_base::beg);
		// バイナリデータを読み込む
		ifs.read((output.data()), output.size());
		ifs.close();

		return output;
	}
	template <class T>
	static std::vector<T> ImportAry(const char* filepath)
	{
		std::vector<char> byte_datas{ FileIO::ImportAry(filepath) };

		size_t data_size{ byte_datas.size() / sizeof(T) };
		std::span data_span(reinterpret_cast<const T*>(byte_datas.data()), data_size);
		return std::vector<T>(data_span.begin(), data_span.end());
	}

	template <class T>
	static T Import(const char* filepath)
	{
		T data;

		std::vector<char> byte_datas{ FileIO::ImportAry(filepath) };

		memcpy_s(&data, sizeof(T), byte_datas.data(), byte_datas.size());

		return data;
	}

};

#if 0

/* --- クラス ---- */

struct Hoge
{
	int a;
	float b;
	int d[3];

	struct HogeHoge
	{
		int e;
		int f;
	} hogehoge;
};

/* --- 実装 ---- */

int main()
{
	Chrono::Start();

	std::vector<Hoge> hoge
	{
		{ 1, 1.1f, { 1, 2, 3}, { 7, 4} },
		{ 2, 1.2f, { 2, 3, 4}, { 6, 4} },
		{ 3, 1.3f, { 3, 4, 5}, { 5, 4} },
		{ 4, 1.4f, { 4, 5, 6}, { 4, 4} },
		{ 5, 1.5f, { 5, 6, 7}, { 3, 4} },
		{ 6, 1.6f, { 6, 7, 8}, { 2, 4} },
		{ 7, 1.7f, { 7, 8, 9}, { 1, 4} },
	};


	FileIO::Export("./test", hoge);

	auto b{ FileIO::Import("./test.bin") };
	auto begin{ reinterpret_cast<Hoge*>(b.data()) };


	for (int i = 0; i < b.size() / sizeof(Hoge); ++i)
	{
		cout << (begin + i)->a << ", ";
		cout << (begin + i)->b << ", ";
		cout << (begin + i)->d[0] << ", ";
		cout << (begin + i)->d[1] << ", ";
		cout << (begin + i)->d[2] << ", ";
		cout << (begin + i)->hogehoge.e << ", ";
		cout << (begin + i)->hogehoge.f << endl;
	}


	return 0;
}

#endif
