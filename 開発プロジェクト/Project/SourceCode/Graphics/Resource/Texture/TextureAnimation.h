#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <filesystem>
#include <d3d11.h>
#include <wrl.h>
#include <string>

/// <summary>
/// 複数の画像のアニメーションを描画
/// </summary>
class TextureAnimation
{
public:

	/// <summary>
	/// コンストラクタ
	/// <para>ファイルパス配列を入力する</para>
	/// </summary>
	/// <param name="paths"></param>
	TextureAnimation(const std::vector<std::wstring>& paths);

	/// <summary>
	/// コンストラクタ
	/// <para>フォルダ内のデータを読み込む(ソートはfilesystemによって行われる)</para>
	/// <para>× 1.png, 2.png ... 11.png, 12.png => 1.png, 11.png, 12.png, 2.png... となる</para>
	/// <para>〇 01.png, 02.png ... 11.png, 12.png => 01.png, 02.png ... 11.png, 12.png </para>
	/// </summary>
	TextureAnimation(const std::string folder_path);
	~TextureAnimation();

private:

	struct Textures
	{
	public:

		Textures(const std::vector<std::wstring>& paths);
		~Textures() = default;

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texcoord;
		};

		struct TextureResource
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
			D3D11_TEXTURE2D_DESC texture2d_desc;
		};

	public:

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;


		std::vector<std::unique_ptr<TextureResource>> texture_resources;

		std::vector<Vertex> vertices;
		const size_t max_vertices;

	public:

		/// <summary>
		/// 指定のテクスチャインデックスの描画をする
		/// </summary>
		/// <param name="index"></param>
		void Render(ID3D11DeviceContext* dc, const size_t index, float dx, float dy, float dw, float dh,
			float tx, float ty, float tw, float th,
			float angle, float r, float g, float b, float a);

		/// <summary>
		/// 指定のテクスチャインデックスの描画をする
		/// </summary>
		/// <param name="index"></param>
		void Render(ID3D11DeviceContext* dc, const size_t index, float dx, float dy, float dw, float dh);

	};

private:

	// 画像データを格納する配列
	std::unique_ptr<Textures> textures;

	/// <summary>
	/// サンプラーステート
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	/// <summary>
	/// ブレンドステート
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

	// 表示する画像の索引番号
	size_t texture_index{ 0 };

	// 一枚の画像を表示させるフレーム数
	float animation_frame { 1.0f / 60 };

	// アニメーションフレームを管理するタイマー
	float animation_timer{ 0.0f };

public:

	void SetAnimationFrame(float set_animation_frame) { animation_frame = set_animation_frame; }

	/// <summary>
	/// 任意のループ時間における一つのアニメーションのフレーム数を計算する
	/// </summary>
	float CalculateAnimationFramePerTime(float loop_time);

	/// <summary>
	/// テクスチャの横幅を取得する(テクスチャのサイズはすべて同じサイズとみなす)
	/// </summary>
	/// <returns></returns>
	float GetTextureWidth() const;

	/// <summary>
	/// テクスチャの縦幅を取得する(テクスチャのサイズはすべて同じサイズとみなす)
	/// </summary>
	/// <returns></returns>
	float GetTextureHeight() const;

public:

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void Update(float elapsed_time);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Render(ID3D11DeviceContext* dc,
		float dx, float dy, float dw, float dh,
		float tx, float ty, float tw, float th,
		float angle, float r, float g, float b, float a);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Render(ID3D11DeviceContext* dc, float dx, float dy, float dw, float dh);
};