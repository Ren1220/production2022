#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>

/* --- �錾 ---- */

class InputLayoutFactory 
{
public:

	static void CreateSkinnedMeshIL(ID3D11Device* device, ID3D11InputLayout** out_input_layout);

};
