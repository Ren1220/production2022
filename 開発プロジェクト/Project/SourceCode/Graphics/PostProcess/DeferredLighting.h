#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>
#include <functional>

#include "../Resource/ConstantBuffer.h"
#include "../Shader/Shader.h"

#include "GeometryBuffer.h"
#include "OmnidirectionalShadow.h"

/* --- 宣言 ---- */


class DeferredLighting
{
public:

	DeferredLighting()
		: width(), height() {}
	~DeferredLighting() = default;

	/// <summary>
	/// <para>点光源、平行光</para>
	/// <para>light_parameterのwの値によって光源の種類、パラメータの内容を決定する</para>
	/// <para>w = 0.0のとき param = { 光の向きxyz  , 0.0 }</para>
	/// <para>w > 0.0のとき param = { 光源の位置xyz, 減衰距離 }</para>
	/// <para></para>
	/// </summary>
	struct Light
	{
		Vector4 light_parameter;
		Vector3 light_color; // 色
		float luminance;	 // 明るさ
	};

	void Initialize(ID3D11Device* device, uint32_t width, uint32_t height);

	void EndProcess(ID3D11DeviceContext* dc);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ジオメトリバッファに書きこむレンダーターゲットビューを設定する
	/// </summary>
	void ActivateGeometryBuffer(ID3D11DeviceContext* dc);

	/// <summary>
	/// Effekseerのエフェクトをジオメトリバッファのemissiveに書き込むための処理
	/// レンダーターゲットの０番にemissiveを設定する。
	/// 深度ステンシルも。
	/// </summary>
	/// <param name="dc"></param>
	void ActivateBufferForEffekseer(ID3D11DeviceContext* dc);

	/// <summary>
	/// 全方位シャドウマップにアクタ、地形の深度書込みを行う。
	/// シャドウイングを行うのは予め登録された点光源のみ
	/// </summary>
	/// <param name="dc"></param>
	void ShadowCast(
		ID3D11DeviceContext* dc);

	/// <summary>
	/// バッファにライティング結果を書き込む
	/// </summary>
	/// <param name="dc"></param>
	void Lighting(ID3D11DeviceContext* dc);

	const Texture& GetTexture() { return lighting_rt->GetTexture(); }

	void ImGuiRender();
	/// <summary>
	/// 光源情報を登録する
	/// (ポインタを弱参照する)
	/// </summary>
	/// <param name="light_data"></param>
	void RegisterLight(const std::shared_ptr<Light>& light_data);

private:

	uint32_t width, height;

	// 光源情報の弱参照
	std::vector<std::weak_ptr<Light>> w_lights;

	GeometryBuffer geometry_buffer;
	std::shared_ptr<RenderTarget> lighting_rt;

	// G-Bufferパスから取得した情報からライティング結果描画する
	Shader lighting_shader; 

	/// <summary>
	/// ライティングに利用する光源の情報定数バッファ
	/// </summary>
	struct CBufferLight
	{
		static constexpr uint32_t SLOT{ 3 };

		Light light[10];
	};
	ConstantBuffer<CBufferLight> light_cb;

	// 点光源の全方位シャドウを行うクラス
	OmnidirectionalShadow shadow_caster;

};
