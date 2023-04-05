#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/Texture/Texture.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Shader/Shader.h"

/* --- 前方宣言 ---- */



/* --- 宣言 ---- */

class ColorFilter
{
public:

	ColorFilter();
	~ColorFilter();

	struct CBufferFilter
	{
		static constexpr UINT SLOT{ 3U };

		float hue_shift;  // 色相
		float saturation; // 彩度
		float brightness; // 明度
		float contrast;   // コントラスト
	};

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	/// <summary>
	/// 引数のテクスチャにカラーフィルターを適用する
	/// </summary>
	void Apply(ID3D11DeviceContext* dc, const Texture& texture);

	/// <summary>
	/// フィルターをかけたテクスチャを取得
	/// </summary>
	/// <returns></returns>
	const Texture& GetTexture() const { return filter_rt->GetTexture(); }

	const CBufferFilter& GetFilterParam() const { return filter_param; }
	void SetFilterParam(const CBufferFilter& p) { filter_param = p; }

private:

	Shader filter_shader;

	CBufferFilter filter_param;
	ConstantBuffer<CBufferFilter> filter_cb;

	std::shared_ptr<RenderTarget> filter_rt;

};
