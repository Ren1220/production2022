#pragma once

/* --- ヘッダーファイルのインクルード ---- */

#include "Shader.h"

#include "Arithmetic/Arithmetic.h"

#include "../Resource/Texture/Texture.h"
#include "../Resource/ConstantBuffer.h"

/* --- クラス定義 ---- */

/// <summary>
/// t0 にテクスチャを設定。b4 に定数バッファを設定。
/// </summary>
class Sprite2DShader : public Shader
{
public:


	Sprite2DShader()
		: Shader() {};
	~Sprite2DShader() override = default;

	/// <summary>
	/// 入力レイアウトの初期化を行う
	/// </summary>
	/// <param name="device"></param>
	void Create(ID3D11Device* device) override;


	// テクスチャがnullの時はダミーテクスチャを使用する
	void Activate(ID3D11DeviceContext* dc) override;
	// テクスチャがnullの時はダミーテクスチャを使用する
	void Activate(ID3D11DeviceContext* dc, const Vector4& material_color);
	// テクスチャがnullの時はダミーテクスチャを使用する
	void Activate(ID3D11DeviceContext* dc, const Texture* texture, const Vector4& material_color);

	struct Vertex
	{
		Vector3 position;
		Vector4 color;
		Vector2 texcoord;
	};

private:


	std::unique_ptr<Texture> dummy_texture;

	struct CBufferMaterial
	{
		Vector4 material_color;
	};
	ConstantBuffer<CBufferMaterial> material_cb;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

};

