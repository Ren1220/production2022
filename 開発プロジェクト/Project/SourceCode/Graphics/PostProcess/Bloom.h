#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>

#include "../Shader/Shader.h"
#include "../Resource/RenderTarget.h"

#include "GaussianBlur.h"

/* --- 宣言 ---- */

/// <summary>
/// 川瀬式ブルーム
/// </summary>
class Bloom
{
public:

	Bloom() 
		: width(), height() {};
	~Bloom() = default;

	void Initialize(
		ID3D11Device* device, 
		uint32_t width, 
		uint32_t height, 
		DXGI_FORMAT format);

	/// <summary>
	/// 引数のテクスチャの輝度を抽出し、ガウスブラーをダウンサンプリングと共に複数回行い、ブルームテクスチャを作成する.
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="scene_texture"></param>
	void Apply(ID3D11DeviceContext* dc, const Texture& scene_texture);

	/// <summary>
	/// ブルームテクスチャを取得する。
	/// </summary>
	/// <returns></returns>
	const Texture& GetTexture() const { return blur_composition_render_target->GetTexture(); }

private:

	uint32_t width, height;

	/// <summary>
	/// 輝度を抽出する
	/// </summary>
	/// <param name="dc">デバイスコンテキスト</param>
	/// <param name="texture">輝度を抽出するテクスチャの</param>
	void SamplingLuminance(ID3D11DeviceContext* dc, const Texture& scene_texture);
	/// <summary>
	/// 輝度を抽出したテクスチャにブラーをかける
	/// </summary>
	/// <param name="dc">デバイスコンテキスト</param>
	virtual void ApplyBlurEffect(ID3D11DeviceContext* dc);
	/// <summary>
	/// ブラーをかけたすべての輝度テクスチャを合成する。
	/// </summary>
	/// <param name="dc"></param>
	void CompositionBlur(ID3D11DeviceContext* dc);

	std::unique_ptr<Shader> luminance_shader;			   // 輝度抽出
	std::shared_ptr<RenderTarget> luminance_render_target; // 輝度抽出

	static constexpr uint32_t blur_sample_num{ 4 };

	std::array<GaussianBlur, blur_sample_num> blur_samples;

	std::shared_ptr<RenderTarget> blur_composition_render_target;
	std::unique_ptr<Shader> blur_composition_shader;

};
