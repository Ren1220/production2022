#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <d2d1_1.h>
#include <dwrite.h>

#include "../Resource/RenderTarget.h"
#include "../Resource/DepthStencil.h"

/* --- 宣言 ---- */

class DirectXUser
{
public:

	DirectXUser();
	~DirectXUser() = default;

	void Initialize(HWND hwnd, UINT width, UINT height);
	void Resize(UINT width, UINT height);
	void Present(UINT sync_interval, UINT flags);

	void ActivateSwapChainBuffer(float width, float height);

	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return device_context.Get(); }
	ID2D1DeviceContext* GetDeviceContext2D() { return device_context2d.Get(); }
	ID2D1Factory1* GetFactory2D() { return factory2d.Get(); }

private:

	void CreateAdapter(IDXGIFactory6* dxgi_factory6);
	void AcquireHighPerformanceAdapter(
		IDXGIFactory6* dxgi_factory6, 
		IDXGIAdapter3** dxgi_adapter3);
	void CreateDevice();
	void CreateSwapChain(HWND hwnd, IDXGIFactory6* dxgi_factory6, UINT width, UINT height);
	void ResizeSwapChain(UINT width, UINT height);
	void CreateFrameBuffer(UINT width, UINT height);

	void CreateDirect2D();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID2D1Device> device2d;
	Microsoft::WRL::ComPtr<ID2D1Factory1> factory2d;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> device_context2d;

	// 1枚の画像の中に複数フレームの画像が描画されてしまい、映像が途中で左右にずれたように見える現象のことです。
	// この現象は、グラフィックカードが送り出すフレームレート（FPS）がディスプレイのリフレッシュレートとずれることで起きます。
	BOOL tearing_supported;
};
