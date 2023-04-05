#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>

/* --- 宣言 ---- */

class InputLayoutFactory 
{
public:

	static void CreateSkinnedMeshIL(ID3D11Device* device, ID3D11InputLayout** out_input_layout);

};
