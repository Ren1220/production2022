#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "Utility/Singleton.h"

#include "Graphics/Resource/Texture/Texture.h"

/* --- 宣言 ---- */

class AreaChange : public Singleton<AreaChange>
{
public:
	 
	AreaChange()
		: fade(), fade_prime(), fade_delta(){}
	~AreaChange() = default;

	void Initialize(ID3D11Device* device, float inital_fade);

	void Update(float elapsed_time);

	void FadeIn(float delta) { fade_prime = 1.0f;  fade_delta = delta; }
	void FadeOut(float delta) { fade_prime = 0.0f; fade_delta = delta; }

	void Render(ID3D11DeviceContext* dc, float width, float height);

	/// <summary>
	/// フェードしきったか
	/// </summary>
	/// <param name="tol">許容値</param>
	/// <returns></returns>
	bool IsFaded(float tol = 0.01f);

	float GetFade() const { return fade; }

private:

	float fade;		    // フェードの値
	float fade_prime;	// フェードの変化後の値
	float fade_delta;	// 一秒あたりの変化量	    
	Texture fade_texture;

};
