#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Utility/Singleton.h"

#include "Graphics/Resource/Texture/Texture.h"

/* --- �錾 ---- */

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
	/// �t�F�[�h����������
	/// </summary>
	/// <param name="tol">���e�l</param>
	/// <returns></returns>
	bool IsFaded(float tol = 0.01f);

	float GetFade() const { return fade; }

private:

	float fade;		    // �t�F�[�h�̒l
	float fade_prime;	// �t�F�[�h�̕ω���̒l
	float fade_delta;	// ��b������̕ω���	    
	Texture fade_texture;

};
