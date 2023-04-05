#include "DirectXUser.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Include/misc.h"

/* --- 定義 ---- */

DirectXUser::DirectXUser()
{
}

void DirectXUser::Initialize(HWND hwnd, UINT width, UINT height)
{
	HRESULT hr{ S_OK };
	UINT create_factory_flags{};

#ifdef _DEBUG
	create_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DirectXファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
	hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	CreateAdapter(dxgi_factory6.Get());
	CreateDevice();
	CreateSwapChain(hwnd, dxgi_factory6.Get(), width, height);
	CreateFrameBuffer(width, height);
	// DirectX3Dデバイス作成後に呼ぶ
	CreateDirect2D();
}

void DirectXUser::Resize(UINT width, UINT height)
{
	ResizeSwapChain(width, height);
	CreateFrameBuffer(width, height);
}

void DirectXUser::Present(UINT sync_interval, UINT flags)
{
	swap_chain->Present(sync_interval, flags);
}

void DirectXUser::ActivateSwapChainBuffer(float width, float height)
{
	constexpr float color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	device_context->ClearRenderTargetView(render_target_view.Get(), color);

	constexpr UINT clear_flags{ D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL };
	device_context->ClearDepthStencilView(depth_stencil_view.Get(), clear_flags, 1.0f, 0);

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	device_context->RSSetViewports(1, &viewport);

	device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void DirectXUser::CreateAdapter(IDXGIFactory6* dxgi_factory6)
{
	AcquireHighPerformanceAdapter(dxgi_factory6, adapter.GetAddressOf());
}

void DirectXUser::AcquireHighPerformanceAdapter(
	IDXGIFactory6* dxgi_factory6, 
	IDXGIAdapter3** dxgi_adapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumerated_adapter;
	for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory6->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumerated_adapter.ReleaseAndGetAddressOf())); ++adapter_index)
	{
		DXGI_ADAPTER_DESC1 adapter_desc;
		hr = enumerated_adapter->GetDesc1(&adapter_desc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
		{
			OutputDebugStringW((std::wstring(adapter_desc.Description) + L" has been selected.\n").c_str());
			OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc.VendorId) + '\n').c_str());
			OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc.DeviceId) + '\n').c_str());
			OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc.SubSysId) + '\n').c_str());
			OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc.Revision) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc.DedicatedVideoMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc.DedicatedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc.SharedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc.AdapterLuid.HighPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc.AdapterLuid.LowPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc.Flags) + '\n').c_str());
			break;
		}
	}
	*dxgi_adapter3 = enumerated_adapter.Detach();
}

void DirectXUser::CreateDevice()
{
	HRESULT hr{ S_OK };

	UINT create_device_flags{ 0 };

	create_device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; //  DirectX11上でDirect2Dを使用するために

#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_levels[]
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL feature_level;
	hr = D3D11CreateDevice(
		adapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
		create_device_flags,
		feature_levels,
		_countof(feature_levels),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),
		&feature_level,
		device_context.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	_ASSERT_EXPR(!(feature_level < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported.");
}

void DirectXUser::CreateSwapChain(
	HWND hwnd, 
	IDXGIFactory6* dxgi_factory6, 
	UINT width, 
	UINT height)
{
	HRESULT hr{ S_OK };

	// 1枚の画像の中に複数フレームの画像が描画されてしまい、映像が途中で左右にずれたように見える現象のことです。
	// この現象は、グラフィックカードが送り出すフレームレート（FPS）がディスプレイのリフレッシュレートとずれることで起きます。
	BOOL allow_tearing = FALSE;
	if (SUCCEEDED(hr))
	{
		hr = dxgi_factory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
	}
	tearing_supported = SUCCEEDED(hr) && allow_tearing;

	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
	swap_chain_desc1.Width  = width;
	swap_chain_desc1.Height = height;
	swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_desc1.Stereo = FALSE;
	swap_chain_desc1.SampleDesc.Count = 1;
	swap_chain_desc1.SampleDesc.Quality = 0;
	swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc1.BufferCount = 2;
	swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc1.Flags = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	hr = dxgi_factory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, NULL, NULL, swap_chain.ReleaseAndGetAddressOf());
#if 0
	swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = dxgi_factory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void DirectXUser::ResizeSwapChain(UINT width, UINT height)
{
	HRESULT hr{ S_OK };

	// ファクトリの作成
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
	hr = swap_chain->GetParent(IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 空のレンダーターゲットビューをデバイスコンテキストに設定する
	ID3D11RenderTargetView* null_rtv{};
	device_context->OMSetRenderTargets(1, &null_rtv, NULL);
	// レンダーターゲットビューの初期化を行う
	render_target_view.Reset();

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{ };
	// スワップチェーンの情報を取得する
	swap_chain->GetDesc(&swap_chain_desc);
	// スワップチェーンのバッファのサイズを変更する
	hr = swap_chain->ResizeBuffers(swap_chain_desc.BufferCount, width, height, swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void DirectXUser::CreateFrameBuffer(UINT width, UINT height)
{
	HRESULT hr{ S_OK };

	// レンダーターゲット
	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, render_target_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシル
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ビューポート
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	device_context->RSSetViewports(1, &viewport);
}

void DirectXUser::CreateDirect2D()
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIDevice2> dxgi_device;
	hr = device.As(&dxgi_device); // キャスト
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D2D1_FACTORY_OPTIONS factory_options{};

#ifdef  _DEBUG
	factory_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif //  _DEBUG

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory_options, factory2d.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = factory2d->CreateDevice(dxgi_device.Get(), device2d.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device2d->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, device_context2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ドライバーがキューに登録できるバック バッファー フレームの最大数。
	hr = dxgi_device->SetMaximumFrameLatency(1);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<IDXGISurface2> dxgi_surface2;
	hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface2));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap1;
	hr = device_context2d->CreateBitmapFromDxgiSurface(dxgi_surface2.Get(),
		D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), d2d_bitmap1.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	device_context2d->SetTarget(d2d_bitmap1.Get());
}
