#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <array>

#include "../Resource/ConstantBuffer.h"
#include "../Resource/RenderTarget.h"
#include "../Shader/Shader.h"

/* --- 宣言 ---- */

class GaussianBlur
{
public:

	GaussianBlur();

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format);
	/// <summary>
	/// 引数のテクスチャにガウスブラーをかけたテクスチャを自身のレンダーターゲットテクスチャに書き込む。
	/// 書き込んだテクスチャは GetTexture() で取得。
	/// </summary>
	/// <param name="texture">このテクスチャが書き換えられるわけではない</param>
	/// <param name="distribution">ボケ具合</param>
	void Apply(ID3D11DeviceContext* dc, const Texture& texture, float distribution = 8.0f);

	const Texture& GetTexture() { return blur_y_render_target->GetTexture(); }

private:

	// ブラーの重みテーブルを計算する
	void CalcWeightTable(float distribution);

private:

	uint32_t width, height;

	static constexpr int num_weights{ 8 }; // 16byteアライメント単位で


	std::array<float, num_weights> weights;

	std::shared_ptr<RenderTarget> blur_x_render_target;
	std::shared_ptr<RenderTarget> blur_y_render_target;

	std::unique_ptr<Shader> blur_x_shader;
	std::unique_ptr<Shader> blur_y_shader;

	struct CBufferWeight
	{
		static constexpr uint32_t slot{ 5 };
		std::array<float, num_weights> weights; // 16byteアライメント単位で
	};
	ConstantBuffer<CBufferWeight> weight_cb;
};
