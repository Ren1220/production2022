#pragma once

/* --- 概要 ---- */

// 全方位シャドウ用のテクスチャを作成するクラス。
// 全方位シャドウとは点光源となる座標を中心として、各軸の正負の方向に対してのビュー行列を作成し、
// 映りこんだ影を描画する対象の深度の書き込みを行うシャドウ。
// テクスチャはジオメトリシェーダーを利用して一度の描画処理でキューブマップに書き込むことができる。
// 現状ディファードレンダリングを利用してライティングのタイミングでシャドウイングも行っている。
// 参考：https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-12-omnidirectional-shadow-mapping

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <array>

#include "Utility/Singleton.h"

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Shader/Shader.h"


/* --- 宣言 ---- */

/// <summary>
/// 全方位シャドウ
/// 周囲六面の深度をバッファに書き込み、深度テクスチャを作成。
/// </summary>
class OmnidirectionalShadow
{
	struct TextureCubeArray;

public:

	OmnidirectionalShadow();
	~OmnidirectionalShadow();

	/// <summary>
	/// 深度書き込みに必要なデータを作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="tex_w">深度を書き込むキューブマップの一面当たりのサイズ</param>
	/// <param name="tex_h">深度を書き込むキューブマップの一面当たりのサイズ</param>
	/// <param name="max_light_count">深度を書き込むライトの最大数</param>
	/// <param name="srv_format">深度テクスチャのシェーダーリソースのフォーマット、他のフォーマットはこの情報から推測される</param>
	void Create(
		ID3D11Device* device, 
		uint32_t tex_w, 
		uint32_t tex_h, 
		uint32_t max_light_count = 5U,
		DXGI_FORMAT srv_format = DXGI_FORMAT_R32_FLOAT);

	/// <summary>
	/// 書き込み開始処理
	/// </summary>
	/// <param name="dc"></param>
	void Activate(ID3D11DeviceContext* dc);

	/// <summary>
	/// レンダーターゲットの初期化、深度値をFLT_MAXにする。
	/// </summary>
	void ClearRTV(ID3D11DeviceContext* dc);

	/// <summary>
	/// レンダーターゲットに書き込んだ情報をキューブマップ配列に書き込む
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="index">キューブマップ配列のインデックス</param>
	void WriteTextureCubeArray(ID3D11DeviceContext* dc, int32_t index);

	/// <summary>
	/// シャドウ書き込み用シェーダを設定してモデルを描画する
	/// </summary>
	/// <param name="dc"></param>
	void RenderModel(ID3D11DeviceContext* dc);

	/// <summary>
	/// 書き込み終了処理
	/// </summary>
	/// <param name="dc"></param>
	void Deactivate(ID3D11DeviceContext* dc);

	void ImGuiRender();

	const std::shared_ptr<RenderTarget>& GetRenderTarget() const { return render_target; }
	const TextureCubeArray& GetShadowCastTextureCubeArray() const { return shadow_cast_texture_cube_array; }

	void SetCBuffer(ID3D11DeviceContext* dc, uint32_t slot);

	/// <summary>
	/// シャドウを書き込むための情報である
	/// 光源の位置を中心とした各軸の正負の方向に対するビュー行列、
	/// プロジェクション行列、カメラの視点（光源の位置）を計算・設定する。
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="light_pos"></param>
	/// <param name="near_z">プロジェクション行列の最近距離</param>
	/// <param name="far_z">プロジェクション行列の最遠距離</param>
	void UpdateCBuffer(
		ID3D11DeviceContext* dc,
		const Vector3& light_pos,
		float near_z,
		float far_z);

	/// <summary>
	/// シャドウイングを行う点光源の最大数
	/// </summary>
	uint32_t GetMaxLightCount() const { return max_light_count; }

private:

	uint32_t max_light_count; // シャドウイングを行う点光源の最大数
	uint32_t width, height;   // シャドウマップの一面当たりのテクスチャサイズ

	/// <summary>
	/// シャドウを書き込むレンダーターゲットの作成
	/// </summary>
	void CreateRenderTarget(ID3D11Device* device, DXGI_FORMAT srv_format);

	/// <summary>
	/// シャドウを書き込むキューブマップの作成
	/// </summary>
	/// <param name="max_array">キューブマップの数</param>
	void CreateTextureCubeArray(ID3D11Device* device, uint32_t max_array, DXGI_FORMAT srv_format);
	
	/// <summary>
	/// 引数の座標に球を配置し、深度情報を書き込む。
	/// </summary>
	void RenderCircle(ID3D11DeviceContext* dc, const Vector3& position, float radius);

	std::shared_ptr<GLTFModel> circle_model;

	std::shared_ptr<RenderTarget> render_target;

	// ジオメトリシェーダーを用いて一度にキューブマップに各方向の深度を書き込む
	Shader shadow_cast_shader; 
	Shader shadow_cast_shader_instanced; // 地形はインスタンス描画なのでそれ用のシェーダー

	struct CBufferCubeMapView
	{
		static constexpr UINT SLOT{ 3 };

		std::array<Matrix, 6> view; // 右、左、上、下、遠、近
		Matrix projection;			// プロジェクション行列は全方向で共通の物を利用できる
		Vector4 eye;
	};
	ConstantBuffer<CBufferCubeMapView> cube_map_view_cb;
	
	struct TextureCubeArray
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	} shadow_cast_texture_cube_array;


#ifdef _DEBUG
	bool is_shadow_cast;
	// ImGuiでキューブマップのテクスチャを参照するためのバッファ
	std::array<Microsoft::WRL::ComPtr<ID3D11Texture2D>, 6>			copy_textures;
	std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 6>	copy_srvs;
#endif // _DEBUG
};
