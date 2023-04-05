#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <array>

#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Camera/Camera.h"
#include "ConstantBuffer.h"

/* --- 宣言 ---- */

/// <summary>
/// レンダーターゲット、深度ステンシル、ビューポートの設定を行うクラス。
/// マルチレンダーターゲットにも対応している
/// </summary>
class FrameBuffer
{
public:

	FrameBuffer() :
		render_targets(), 
		depth_stencil(), 
		width(), height(), clear_flag(),
		viewports(), frame_buffer_cache() {}

	virtual ~FrameBuffer() = default;

	void Initialize(ID3D11Device* device);
	void EndProcess(ID3D11DeviceContext* dc);

	/// <summary>
	/// デバイスコンテキストに設定したシェーダリソースビューをクリアする。 
	/// </summary>
	void SetNullSRV(ID3D11DeviceContext* dc);
	/// <summary>
	/// デバイスコンテキストに設定したレンダーターゲットビューすべてnullにする。
	/// 現在設定しているレンダーターゲット、デプスステンシルもクリアする。
	/// </summary>
	void SetNullRTV(ID3D11DeviceContext* dc);

public:

	/// <summary>
	/// 設定したレンダーターゲット、デプスステンシルビューをデバイスコンテキストにまとめて設定する。
	/// 設定されてない部分も nullptr を設定するため、
	/// すべてのレンダーターゲット、デプスステンシルビューが書き換えられる。
	/// </summary>
	void ActivateRenderTargets(ID3D11DeviceContext* dc);
	void ActivateViewPorts(ID3D11DeviceContext* dc, uint32_t num_viewport);

	/// <summary>
	/// 現在設定しているレンダーターゲット、デプスステンシルビューの初期化
	/// </summary>
	void ClearCurrentRTVAndDSV(ID3D11DeviceContext* dc, UINT depth_stencil_clear_flag = (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL));

	/// <summary>
	/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
	/// </summary>
	void PrepareRenderTargets(const std::array<std::shared_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT>& render_targets) { this->render_targets = render_targets; }

	/// <summary>
	/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
	/// 引数の順番にインデックスにレンダーターゲットを設定する。
	/// </summary>
	template <class... Args>
	void PrepareRenderTargets(Args const&... args);

	/// <summary>
	/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
	/// </summary>
	void PrepareDepthStencil(const std::shared_ptr<DepthStencil>& depth_stencil) { this->depth_stencil = depth_stencil; }

	/// <summary>
	/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
	/// </summary>
	void PrepareViewports(const std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX>& viewport) { viewports = viewport; }
	
	/// <summary>
	/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
	/// </summary>
	void PrepareViewport(float width, float height) { viewports[0] = { 0.0f, 0.0f, width, height, 0.0f, 1.0f }; }


	const std::shared_ptr<RenderTarget>& GetRenderTarget(uint32_t index) { return render_targets[index]; }

	/// <summary>
	/// ビューポート、深度ステンシル、レンダーターゲットをデバイスコンテキストに設定する。
	/// 各情報はバッファに保持される。
	/// 設定した後、ビューポート、レンダーターゲットは初期化される（カラーは(0)）
	/// 
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="depth_stencil"></param>
	/// <param name="depth_stencil_clear_flag"></param>
	/// <param name="...args"></param>
	template <class ...Args>
	void Set(
		ID3D11DeviceContext* dc,
		float width, 
		float height, 
		const std::shared_ptr<DepthStencil>& depth_stencil,
		UINT depth_stencil_clear_flag, 
		Args const&... args);

	void ResetCache(ID3D11DeviceContext* dc, bool is_clear_ds);

private:

	// デバイスコンテキストに設定するレンダーターゲット・デプスステンシルビューを保持する
	// ここを仲介することで、ある程度管理しやすくなる。
	std::array<std::shared_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> render_targets;
	std::shared_ptr<DepthStencil>	 depth_stencil;
	std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX> viewports;

	float width, height;
	UINT clear_flag;

	class Cache
	{
	public:

		float width, height;
		UINT clear_flag;

		std::array<std::weak_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> w_render_targets;
		std::weak_ptr<DepthStencil>	 w_depth_stencil;
		std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX> viewports;
	} frame_buffer_cache;

public:

	/// <summary>
	/// シーン定数バッファとして1番スロットに送るデータを持つカメラを設定する。
	/// </summary>
	/// <param name="camera"></param>
	void PrepareCamera(const std::shared_ptr<Camera>& camera) { frame_camera = camera; }

	/// <summary>
	/// 設定したカメラ情報によるシーン定数バッファを１番スロットで定数バッファ管理クラスに送信し、
	/// デバイスコンテキストに設定する。
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="camera">nullptrの場合は以前に設定したカメラ。このクラスが持つカメラバッファを上書きする。</param>
	void SetSceneCB(ID3D11DeviceContext* dc, const std::shared_ptr<Camera>& camera);

	std::shared_ptr<Camera> GetFrameCamera() { return frame_camera; }

private:

	/// <summary>
	/// シーン定数バッファ構造体
	/// </summary>
	struct CBufferScene
	{
		static constexpr uint32_t slot{ 1 };

		Matrix view;			 // ビュー変換行列
		Matrix projection;		 // プロジェクション変換行列
		Matrix view_inverse;	 // ビュー逆行列
		Vector4 camera_position; // カメラの位置
	};
	ConstantBuffer<CBufferScene> scene_cb;
	std::shared_ptr<Camera> frame_camera;


};

/// <summary>
/// デバイスコンテキストに送る為のバッファに設定しただけで、デバイスコンテキストに設定されているわけではない
/// 引数の順番にインデックスにレンダーターゲットを設定する。
/// </summary>
template<class... Args>
inline void FrameBuffer::PrepareRenderTargets(Args const&... args)
{
	_ASSERT_EXPR(sizeof...(args) < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, L"設定できるレンダーターゲットの数を超えています。");

	// 配列の初期化
	for (auto& rt : render_targets) { rt.reset(); }

	for (uint32_t ri = 0;
		 const auto& rt : std::initializer_list<std::shared_ptr<RenderTarget>>{args...})
	{
		render_targets[ri++] = rt;
	}
}

template<class ...Args>
inline void FrameBuffer::Set(
	ID3D11DeviceContext* dc,
	float width, 
	float height, 
	const std::shared_ptr<DepthStencil>& depth_stencil,
	UINT depth_stencil_clear_flag, 
	Args const&...args)
{
	// キャッシュ
	frame_buffer_cache = {};

	frame_buffer_cache.width = this->width;
	frame_buffer_cache.height = this->height;
	frame_buffer_cache.clear_flag = this->clear_flag;
	frame_buffer_cache.viewports = this->viewports;
	frame_buffer_cache.w_depth_stencil = this->depth_stencil;
	
	for (uint32_t ri = 0; const auto& rt : render_targets)
	{
		if (rt == nullptr) continue;
		frame_buffer_cache.w_render_targets[ri++] = rt;
	}

	this->width = width;
	this->height = height;
	this->clear_flag = depth_stencil_clear_flag;

	PrepareViewport(width, height);
	PrepareDepthStencil(depth_stencil);
	PrepareRenderTargets(args...);

	ClearCurrentRTVAndDSV(dc, depth_stencil_clear_flag);

	ActivateViewPorts(dc, 1);
	ActivateRenderTargets(dc);

}
