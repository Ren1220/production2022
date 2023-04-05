#include "TextureRenderer.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <array>

#include "../../General/DXHelperUsers.h"

/* --- 定義 ---- */

void TextureRenderer::Initialize(ID3D11Device* device)
{
	constexpr uint32_t max_instance_count{ 10000U };

	// 頂点バッファの作成
	{
		std::vector<Vertex> init(max_instance_count * 4);

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(Vertex) * 4 * max_instance_count;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = init.data();

		HRESULT hr{ device->CreateBuffer(&desc, &data, vertex_buffer.ReleaseAndGetAddressOf()) };
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// シェーダー
	{
		D3D11_INPUT_ELEMENT_DESC desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		sprite_alpha_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso", input_layout.ReleaseAndGetAddressOf(), desc, _countof(desc)); // input_layoutは一回でいい
		sprite_alpha_shader.CreatePSFromCSO(device, "Shader/Sprite2DPS.cso");
		sprite_alpha_shader.CreateBlendState(device, BSMode::Alpha);
		sprite_alpha_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

		sprite_add_shader.CreatePSFromCSO(device, "Shader/Sprite2DPS.cso");
		sprite_add_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso");
		sprite_add_shader.CreateBlendState(device, BSMode::Add);
		sprite_alpha_shader.CreateRasterizerState(device, D3D11_FILL_SOLID);
		sprite_add_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	}

	InitializeBatch(device);
}

void TextureRenderer::EndProcess(ID3D11DeviceContext* dc)
{

}

void TextureRenderer::BeginDraw(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Activate(dc);
	dc->IASetInputLayout(input_layout.Get());
}

void TextureRenderer::BeginDraw(ID3D11DeviceContext* dc, BSMode alpha_or_add)
{
	switch (alpha_or_add)
	{
	case BSMode::Alpha:
		sprite_alpha_shader.Activate(dc);
		break;
	case BSMode::Add:
		sprite_add_shader.Activate(dc);
		break;
	default:
		_ASSERT_EXPR(0, L"未作成のブレンドモード");
		break;
	}

	dc->IASetInputLayout(input_layout.Get());
}

void TextureRenderer::Draw(
	ID3D11DeviceContext* dc,
	const Texture& texture,
	const Vector2& position, 
	const Vector2& size,
	const Vector2& image_offset, 
	const Vector2& image_size,
	const Vector4& color, 
	TexturePivot texture_pivot)
{

	// 頂点情報の更新
	UpdateVertices(dc, texture, position, size, image_offset, image_size, color, texture_pivot);

	// 頂点バッファの設定
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// プリミティブトポロジーの設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->PSSetShaderResources(0, 1, texture.GetView().GetAddressOf());
	dc->VSSetShaderResources(0, 1, texture.GetView().GetAddressOf());

	// 描画
	dc->Draw(4, 0);
}

void TextureRenderer::Draw(
	ID3D11DeviceContext* dc, 
	const Texture& texture, 
	const Vector2& position, 
	const Vector2& size, 
	const Vector4& color, 
	TexturePivot texture_pivot)
{
	float x{ static_cast<float>(texture.GetDesc().Width) };
	float y{ static_cast<float>(texture.GetDesc().Height) };
	Draw(dc, texture, position, size, { 0, 0 }, { x, y }, color, texture_pivot);
}

void TextureRenderer::Draw(
	ID3D11DeviceContext* dc,
	const Texture& texture,
	const Vector2& position, 
	TexturePivot texture_pivot)
{
	float x{ static_cast<float>(texture.GetDesc().Width) };
	float y{ static_cast<float>(texture.GetDesc().Height) };
	Draw(dc, texture, position, { x, y }, { 0, 0 }, { x, y }, {1, 1, 1, 1}, texture_pivot);
}


void TextureRenderer::Draw(
	ID3D11DeviceContext* dc, 
	const Texture& texture)
{
	float x{ static_cast<float>(texture.GetDesc().Width) };
	float y{ static_cast<float>(texture.GetDesc().Height) };
	Draw(dc, texture, {0, 0}, { x, y }, {0, 0}, { x, y }, {1, 1, 1, 1}, TexturePivot::LeftTop);
}

void TextureRenderer::Draw(
	ID3D11DeviceContext* dc, 
	ID3D11ShaderResourceView* const* shader_resource_view, 
	ID3D11Texture2D* texture2d)
{
	// テクスチャ情報の取得
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d->GetDesc(&texture2d_desc);
	Vector2 texture_size{ static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height) };

	// 頂点情報の更新
	UpdateVertices(dc, { 0, 0 }, texture_size, { 0, 0 }, texture_size, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height), { 1, 1, 1, 1 }, TexturePivot::LeftTop);

	// 頂点バッファの設定
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// プリミティブトポロジーの設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->PSSetShaderResources(0, 1, shader_resource_view);
	dc->VSSetShaderResources(0, 1, shader_resource_view);

	// 描画
	dc->Draw(4, 0);
}

void TextureRenderer::Draw(
	ID3D11DeviceContext* dc, 
	ID3D11ShaderResourceView* const* shader_resource_view, 
	uint32_t num_views,
	float width, 
	float height)
{
	// 頂点情報の更新
	UpdateVertices(
		dc,
		{ 0, 0 },
		{ width, height },
		{ 0, 0 },
		{ width, height },
		width, height,
		{ 1, 1, 1, 1 },
		TexturePivot::LeftTop);

	// 頂点バッファの設定
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// プリミティブトポロジーの設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->VSSetShaderResources(0, num_views, shader_resource_view);
	dc->PSSetShaderResources(0, num_views, shader_resource_view);
	
	// 描画
	dc->Draw(4, 0);
}

void TextureRenderer::EndDraw(ID3D11DeviceContext* dc) const
{
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
	dc->HSSetShader(nullptr, nullptr, 0);
	dc->DSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}
void TextureRenderer::UpdateVertices(
	ID3D11DeviceContext* dc, 
	const Texture& texture, 
	const Vector2& position, 
	const Vector2& size, 
	const Vector2& image_offset, 
	const Vector2& image_size, 
	const Vector4& color, 
	TexturePivot texture_pivot)
{
	Vector3 positions[4]
	{
		{ position.x		 , position.y		  , 0.0f },
		{ position.x + size.x, position.y		  , 0.0f },
		{ position.x		 , position.y + size.y, 0.0f },
		{ position.x + size.x, position.y + size.y, 0.0f }
	};

	Vector2 texcoords[4]
	{
		{ image_offset.x			   , image_offset.y		   },
		{ image_offset.x + image_size.x, image_offset.y		   },
		{ image_offset.x			   , image_offset.y + image_size.y },
		{ image_offset.x + image_size.x, image_offset.y + image_size.y }
	};

	if (texture_pivot == TexturePivot::Center)
	{
		for (auto& position : positions)
		{
			position.x -= size.x * 0.5f;
			position.y -= size.y * 0.5f;
		}
	}

	// スクリーン座標から正規化デバイス座標に変換する
	D3D11_VIEWPORT viewport;
	UINT num_viewport{ 1 };
	dc->RSGetViewports(&num_viewport, &viewport);

	for (auto& position : positions)
	{
		position.x = 2.0f * position.x / viewport.Width - 1.0f;
		position.y = 1.0f - 2.0f * position.y / viewport.Height;
	}

	float tex_width{ static_cast<float>(texture.GetDesc().Width) };
	float tex_height{ static_cast<float>(texture.GetDesc().Height) };

	for (auto& texcoord : texcoords)
	{
		texcoord.x = texcoord.x / tex_width;
		texcoord.y = texcoord.y / tex_height;
	}

	// 頂点バッファの情報を更新する
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	HRESULT hr = dc->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), L"頂点バッファの編集を開始できませんでした");

	Vertex* v = reinterpret_cast<Vertex*>(mapped_buffer.pData);
	for (int i = 0; i < 4; i++)
	{
		v[i].position = positions[i];
		v[i].color = color;
		v[i].texcoord = texcoords[i];
	}

	// 更新終了
	dc->Unmap(vertex_buffer.Get(), 0);

}

void TextureRenderer::UpdateVertices(ID3D11DeviceContext* dc, 
	const Vector2& position, const Vector2& size, 
	const Vector2& image_offset, const Vector2& image_size, 
	float texture_width, float texture_height, 
	const Vector4& color, TexturePivot texture_pivot)
{
	Vector3 positions[4]
	{
		{ position.x		 , position.y		  , 0.0f },
		{ position.x + size.x, position.y		  , 0.0f },
		{ position.x		 , position.y + size.y, 0.0f },
		{ position.x + size.x, position.y + size.y, 0.0f }
	};

	Vector2 texcoords[4]
	{
		{ image_offset.x			   , image_offset.y		   },
		{ image_offset.x + image_size.x, image_offset.y		   },
		{ image_offset.x			   , image_offset.y + image_size.y },
		{ image_offset.x + image_size.x, image_offset.y + image_size.y }
	};

	if (texture_pivot == TexturePivot::Center)
	{
		for (auto& position : positions)
		{
			position.x -= size.x * 0.5f;
			position.y -= size.y * 0.5f;
		}
	}

	// スクリーン座標から正規化デバイス座標に変換する
	D3D11_VIEWPORT viewport;
	UINT num_viewport{ 1 };
	dc->RSGetViewports(&num_viewport, &viewport);

	for (auto& position : positions)
	{
		position.x = 2.0f * position.x / viewport.Width - 1.0f;
		position.y = 1.0f - 2.0f * position.y / viewport.Height;
	}

	for (auto& texcoord : texcoords)
	{
		texcoord.x = texcoord.x / texture_width;
		texcoord.y = texcoord.y / texture_height;
	}

	// 頂点バッファの情報を更新する
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	HRESULT hr = dc->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), L"頂点バッファの編集を開始できませんでした");

	Vertex* v = reinterpret_cast<Vertex*>(mapped_buffer.pData);
	for (int i = 0; i < 4; i++)
	{
		v[i].position = positions[i];
		v[i].color = color;
		v[i].texcoord = texcoords[i];
	}

	// 更新終了
	dc->Unmap(vertex_buffer.Get(), 0);


}

void TextureRenderer::InitializeBatch(ID3D11Device* device, uint32_t max_instance_count)
{
	HRESULT hr{ S_OK };

	this->max_instance_count = max_instance_count;

	// 頂点情報
	{
		BatchVertex vertices[4]
		{
			{ Vector3(0, 0, 0), Vector2(0, 0) },
			{ Vector3(1, 0, 0), Vector2(1, 0) },
			{ Vector3(0, 1, 0), Vector2(0, 1) },
			{ Vector3(1, 1, 0), Vector2(1, 1) },
		};

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(BatchVertex) * 4U;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = vertices;

		hr = device->CreateBuffer(&desc, &data, batch_vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// インスタンス情報
	{
		std::vector<BatchInstance> datas(max_instance_count);

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(BatchInstance) * max_instance_count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = datas.data();

		device->CreateBuffer(&desc, &data, batch_instance_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// シェーダー情報、入力レイアウト情報
	{
		D3D11_INPUT_ELEMENT_DESC desc[]
		{
			{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};

		batch_alpha_shader.CreateVSFromCSO(device, "Shader/SpriteBatchVS.cso", batch_input_layout.ReleaseAndGetAddressOf(), desc, _countof(desc));
		batch_alpha_shader.CreatePSFromCSO(device, "Shader/SpriteBatchPS.cso");
		batch_alpha_shader.CreateBlendState(device, BSMode::Alpha);
		batch_alpha_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
		batch_alpha_shader.CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_NONE);

	}
}

void TextureRenderer::BeginDrawBatch(ID3D11DeviceContext* dc, BSMode alpha_or_add)
{
	batch_alpha_shader.Activate(dc);

	dc->IASetInputLayout(batch_input_layout.Get());

	draw_batch_count = 0U;

	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	HRESULT hr = dc->Map(batch_instance_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), L"頂点バッファの編集を開始できませんでした");
	p_batch_instance_datas = reinterpret_cast<BatchInstance*>(mapped_buffer.pData);
}

void TextureRenderer::DrawBatch(
	ID3D11DeviceContext* dc,
	const Texture& texture,
	const Vector2& position,
	const Vector2& size,
	const Vector2& tex_pos,
	const Vector2& tex_size,
	const Vector4& color)
{
	float tw{ tex_size.x };
	float th{ tex_size.y };

	// スクリーン座標から正規化デバイス座標に変換する
	D3D11_VIEWPORT viewport;
	UINT num_viewport{ 1 };
	dc->RSGetViewports(&num_viewport, &viewport);

	float w = 2.0f / viewport.Width;
	float h = -2.0f / viewport.Height;

	float tex_w{ static_cast<float>(texture.GetDesc().Width) };
	float tex_h{ static_cast<float>(texture.GetDesc().Height) };

	float sx{ size.x / tex_w };
	float sy{ size.y / tex_h };

	p_batch_instance_datas[draw_batch_count].ndcTransform._11 = w * sx * tw;
	p_batch_instance_datas[draw_batch_count].ndcTransform._12 = 0.0f;// h* sx* tw;
	p_batch_instance_datas[draw_batch_count].ndcTransform._13 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._14 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._21 = 0.0f;//w * sy * th;
	p_batch_instance_datas[draw_batch_count].ndcTransform._22 = h * sy * th;
	p_batch_instance_datas[draw_batch_count].ndcTransform._23 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._24 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._31 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._32 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._33 = 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._34 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._41 = w * (position.x + sx) - 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._42 = h * (position.y + sy) + 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._43 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._44 = 1.0f;


	float u = tw / tex_w;
	float v = th / tex_h;

	p_batch_instance_datas[draw_batch_count].texcoordTransform = { tex_pos.x / tex_w, tex_pos.y / tex_h, u, v };
	p_batch_instance_datas[draw_batch_count].color = color;

	draw_batch_count++;
}

void TextureRenderer::DrawBatch(
	ID3D11DeviceContext* dc, 
	const Texture& texture, 
	const Vector2& position, const Vector2& size,
	const Vector2& tex_pos,  const Vector2& tex_size,
	const Vector2& center, float angle,
	const Vector4& color)
{
	// スクリーン座標から正規化デバイス座標に変換する
	D3D11_VIEWPORT viewport;
	UINT num_viewport{ 1 };
	dc->RSGetViewports(&num_viewport, &viewport);

	float w = 2.0f / viewport.Width;
	float h = -2.0f / viewport.Height;

	float tex_w{ static_cast<float>(texture.GetDesc().Width) };
	float tex_h{ static_cast<float>(texture.GetDesc().Height) };

	const auto& [tw, th] { tex_size };
	auto [cx, cy] { center };

	float sx{ size.x / tex_w };
	float sy{ size.y / tex_h };

	cx *= sx;
	cy *= sy;

	float cos{ cosf(angle) };
	float sin{ sinf(angle) };

	p_batch_instance_datas[draw_batch_count].ndcTransform._11 = w * sx * tw * cos;
	p_batch_instance_datas[draw_batch_count].ndcTransform._12 = h * sx * tw * sin;
	p_batch_instance_datas[draw_batch_count].ndcTransform._13 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._14 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._21 = w * sy * th * -sin;
	p_batch_instance_datas[draw_batch_count].ndcTransform._22 = h * sy * th * cos;
	p_batch_instance_datas[draw_batch_count].ndcTransform._23 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._24 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._31 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._32 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._33 = 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._34 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._41 = w * (-cx * cos + -cy * -sin + position.x) - 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._42 = h * (-cx * sin + -cy * cos + position.y) + 1.0f;
	//p_batch_instance_datas[draw_batch_count].ndcTransform._41 = w * (position.x + sx) - 1.0f;
	//p_batch_instance_datas[draw_batch_count].ndcTransform._42 = h * (position.y + sy) + 1.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._43 = 0.0f;
	p_batch_instance_datas[draw_batch_count].ndcTransform._44 = 1.0f;


	float u = tw / tex_w;
	float v = th / tex_h;

	p_batch_instance_datas[draw_batch_count].texcoordTransform = { tex_pos.x / tex_w, tex_pos.y / tex_h, u, v };
	p_batch_instance_datas[draw_batch_count].color = color;

	draw_batch_count++;
}

void TextureRenderer::EndDrawBatch(ID3D11DeviceContext* dc, const Texture& texture)
{
	// 更新終了
	dc->Unmap(batch_instance_buffer.Get(), 0);

	// 頂点バッファの設定
	UINT strides[2]{ sizeof(BatchVertex), sizeof(BatchInstance)};
	UINT offsets[2]{ 0, 0 };
	ID3D11Buffer* buffers[2]{ batch_vertex_buffer.Get(), batch_instance_buffer.Get() };
	dc->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	// プリミティブトポロジーの設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->PSSetShaderResources(0, 1, texture.GetView().GetAddressOf());
	dc->VSSetShaderResources(0, 1, texture.GetView().GetAddressOf());

	// 描画
	dc->DrawInstanced(4U, draw_batch_count, 0, 0);
}

