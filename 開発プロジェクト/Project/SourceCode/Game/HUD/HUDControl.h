#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Resource/Texture/Texture.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class HUDControl
{
public:

	HUDControl();
	~HUDControl();

	void Initialize(ID3D11Device* device);

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* dc);

private:

	/// <summary>
	/// 一部のフラグはプレイヤーや敵の中でフラグを操作している
	/// </summary>
	void UpdateTutorial(float elapsed_time);

	void RenderItemText(
		ID3D11DeviceContext* dc, 
		const std::shared_ptr<Actor>& actor, 
		bool& can_render_tutorial);

	void RenderTutorial(ID3D11DeviceContext* dc);

	Texture text_frame;

	void DrawMessage(ID3D11DeviceContext* dc, const wchar_t* text);

	float timer;
};
