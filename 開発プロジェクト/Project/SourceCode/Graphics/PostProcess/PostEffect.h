#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "../Resource/RenderTarget.h"
#include "../Shader/Shader.h"

#include "Bloom.h"
#include "ColorFilter.h"

/* --- 宣言 ---- */

class PostEffect
{
public:

	PostEffect()
		: width(), height() {};
	~PostEffect() = default;

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	void ImGuiRender();

	void Apply(
		ID3D11DeviceContext* dc,
		const Texture& scene_texture);

	const Texture& GetTexture() const;

	ColorFilter& GetColorFilter() { return color_filter; }

private:

	uint32_t width, height;

	Shader composition_shader;
	std::shared_ptr<RenderTarget> composition_rt;

	Bloom bloom;
	ColorFilter color_filter;

};
