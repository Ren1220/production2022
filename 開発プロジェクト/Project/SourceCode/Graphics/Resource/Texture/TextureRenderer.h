#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <wrl.h>

#include "Arithmetic/Arithmetic.h"

#include "../../Shader/Shader.h"
#include "../RenderState.h"
#include "Texture.h"

/* --- 宣言 ---- */

enum class TexturePivot
{
	LeftTop,
	Center
};

class TextureRenderer
{
	struct Vertex
	{
		Vector3 position;
		Vector4 color;
		Vector2 texcoord;
	};

public:

	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D11Device* device);
	/// <summary>
	/// 終了処理
	/// </summary>
	void EndProcess(ID3D11DeviceContext* dc);
	/// <summary>
	/// 外部シェーダーを設定する。入力レイアウトはこのクラスの物を設定するので、セマンティクスを合わせる。
	/// </summary>
	void BeginDraw(ID3D11DeviceContext* dc, Shader* shader);
	/// <summary>
	/// このクラス内のシェーダーを利用する。デフォルトはαブレンド
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="alpha_or_add">この二つ以外は設定できない。必要なら作る</param>
	void BeginDraw(ID3D11DeviceContext* dc, BSMode alpha_or_add = BSMode::Alpha);


	
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="image_offset"></param>
	/// <param name="image_size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">座標の基準点</param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture, 
		const Vector2& position, 
		const Vector2& size, 
		const Vector2& image_offset, 
		const Vector2& image_size,
		const Vector4& color, 
		TexturePivot texture_pivot);
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">座標の基準点</param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture, 
		const Vector2& position,
		const Vector2& size, 
		const Vector4& color, 
		TexturePivot texture_pivot);
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="texture_pivot">座標の基準点</param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture,  
		const Vector2& position, 
		TexturePivot texture_pivot);
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture);
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="texture2d"></param>
	void Draw(
		ID3D11DeviceContext* dc, 
		ID3D11ShaderResourceView*const* shader_resource_view,
		ID3D11Texture2D* texture2d);
	/// <summary>
	/// 描画実行
	/// </summary>
	/// <param name="dc"></param>
	void Draw(
		ID3D11DeviceContext* dc, 
		ID3D11ShaderResourceView* const* shader_resource_view, 
		uint32_t num_views,
		float width, float height);
	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="dc"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="...args"></param>
	template <class ...Args>
	void Draw(
		ID3D11DeviceContext* dc,
		uint32_t width,
		uint32_t height,
		Args ...args);

	/// <summary>
	/// 描画終了
	/// </summary>
	void EndDraw(ID3D11DeviceContext* dc) const;


private:

	/// <summary>
	/// 頂点情報の更新
	/// </summary>
	void UpdateVertices(
		ID3D11DeviceContext* dc, 
		const Texture& texture,
		const Vector2& position,
		const Vector2& size,
		const Vector2& image_offset, 
		const Vector2& image_size,
		const Vector4& color,
		TexturePivot texture_pivot);
	/// <summary>
	/// 頂点情報の更新
	/// </summary>
	void UpdateVertices(
		ID3D11DeviceContext* dc,
		const Vector2& position, 
		const Vector2& size,
		const Vector2& image_offset, 
		const Vector2& image_size,
		float texture_width, 
		float texture_height,
		const Vector4& color, 
		TexturePivot texture_pivot);

private:

	uint32_t draw_batch_count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Shader sprite_alpha_shader;
	Shader sprite_add_shader;

public:


	/// <summary>
	/// 一枚のテクスチャをまとめて描画する準備
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="alpha_or_add"></param>
	void BeginDrawBatch(ID3D11DeviceContext* dc, BSMode alpha_or_add = BSMode::Alpha);

	/// <summary>
	/// まとめて描画を行うバッファに書き込んでいく
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="tex_pos"></param>
	/// <param name="tex_size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">座標の基準点</param>
	void DrawBatch(
		ID3D11DeviceContext* dc,
		const Texture& texture,
		const Vector2& position,
		const Vector2& size,
		const Vector2& tex_pos,
		const Vector2& tex_size,
		const Vector4& color);

	/// <summary>
	/// まとめて描画を行うバッファに書き込んでいく
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="tex_pos"></param>
	/// <param name="tex_size"></param>
	/// <param name="center"></param>
	/// <param name="angle">radian</param>
	/// <param name="color"></param>
	void DrawBatch(
		ID3D11DeviceContext* dc,
		const Texture& texture,
		const Vector2& position,
		const Vector2& size,
		const Vector2& tex_pos,
		const Vector2& tex_size,
		const Vector2& center,
		float angle,
		const Vector4& color);

	/// <summary>
	/// バッファに書き込んだデータを実際に描画する
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	void EndDrawBatch(ID3D11DeviceContext* dc, const Texture& texture);

private:

	struct BatchVertex
	{
		Vector3 position;
		Vector2 texcoord;
	};

	struct BatchInstance
	{
		Matrix  ndcTransform;
		Vector4 texcoordTransform;
		Vector4 color;
	};

	uint32_t max_instance_count;

	void InitializeBatch(ID3D11Device* device, uint32_t max_instance_count = 10000U);

	Microsoft::WRL::ComPtr<ID3D11Buffer> batch_vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> batch_instance_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> batch_input_layout;
	BatchInstance* p_batch_instance_datas;

	Shader batch_alpha_shader;
};

template<class ...Args>
inline void TextureRenderer::Draw(
	ID3D11DeviceContext* dc,
	uint32_t width, 
	uint32_t height, 
	Args ...args)
{
	Vector2 texture_size{ static_cast<float>(width), static_cast<float>(height) };

	// 頂点情報の更新
	UpdateVertices(dc, { 0, 0 }, texture_size, { 0, 0 }, texture_size, texture_size.x, texture_size.y, { 1, 1, 1, 1 }, TexturePivot::LeftTop);

	// 頂点バッファの設定
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// プリミティブトポロジーの設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	uint32_t srv_count{ static_cast<uint32_t>(sizeof...(args)) };

	std::vector<ID3D11ShaderResourceView*> srvs(srv_count);
	for (uint32_t ti = 0;
		 const Texture& t : std::initializer_list<Texture>{ args... })
	{
		srvs[ti++] = t.GetView().Get();
	}

	dc->PSSetShaderResources(0, srv_count - 1U, srvs.data());
	dc->VSSetShaderResources(0, srv_count - 1U, srvs.data());

	// 描画
	dc->Draw(4, 0);
}
