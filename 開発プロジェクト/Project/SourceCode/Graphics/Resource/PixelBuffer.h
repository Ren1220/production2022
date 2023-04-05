#pragma once

// 参考　DirectX-Graphics-Samples/MiniEngine/Core/PixelBuffer.cpp

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>

/* --- 宣言 ---- */

class PixelBuffer
{
public:

	static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);

	static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);

	static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
	static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
	static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);

	static size_t BytesPerPixel(DXGI_FORMAT format);

};
