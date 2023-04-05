#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "Arithmetic/Arithmetic.h"

#include "../Resource/RenderTarget.h"
#include "../Resource/DepthStencil.h"

/* --- 宣言 ---- */

class GeometryBuffer
{
public:

	GeometryBuffer();
	~GeometryBuffer() = default;

	/// <summary>
	/// バッファの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="width">テクスチャのサイズ</param>
	/// <param name="height">テクスチャのサイズ</param>
	void Create(ID3D11Device* device, uint32_t width, uint32_t height);
	/// <summary>
	/// 各バッファのレンダーターゲット、深度ステンシルを設定して、Gバッファへの書込みを開始する。
	/// レンダーターゲットの順番は、クラスの変数の順番
	/// </summary>
	/// <param name="dc"></param>
	void ActivateGeometry(ID3D11DeviceContext* dc);

	void ImGuiRender();

public:

	uint32_t width, height;

	std::shared_ptr<DepthStencil> depth_stencil;

	std::shared_ptr<RenderTarget> color_rt;	 			 // 色バッファ
	std::shared_ptr<RenderTarget> roughness_metallic_rt; // ラフネス・メタリックバッファ
	std::shared_ptr<RenderTarget> emmisive_rt;	 		 // 自己発光バッファ
	std::shared_ptr<RenderTarget> occlusion_rt; 		 // 陰影バッファ
	std::shared_ptr<RenderTarget> normal_rt;	 		 // 法線バッファ
	std::shared_ptr<RenderTarget> world_position_rt;	 // 3D位置バッファ
	std::shared_ptr<RenderTarget> depth_rt;	 			 // 深度バッファ
};
