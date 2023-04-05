#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>

#include "Utility/Singleton.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- 宣言 ---- */

class TitleSystem : public Singleton<TitleSystem>
{
public:

	void Initialize(ID3D11Device* device);

	void Render(ID3D11DeviceContext* dc);

	bool IsTitle() const { return is_title; }
	void SetTitleFlag(bool b) { is_title = b; }

private:

	bool is_title;
	Texture text_frame;
	Texture title;

};
