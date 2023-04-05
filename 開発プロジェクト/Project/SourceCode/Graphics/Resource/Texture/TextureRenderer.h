#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>

#include "Arithmetic/Arithmetic.h"

#include "../../Shader/Shader.h"
#include "../RenderState.h"
#include "Texture.h"

/* --- �錾 ---- */

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
	/// ������
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D11Device* device);
	/// <summary>
	/// �I������
	/// </summary>
	void EndProcess(ID3D11DeviceContext* dc);
	/// <summary>
	/// �O���V�F�[�_�[��ݒ肷��B���̓��C�A�E�g�͂��̃N���X�̕���ݒ肷��̂ŁA�Z�}���e�B�N�X�����킹��B
	/// </summary>
	void BeginDraw(ID3D11DeviceContext* dc, Shader* shader);
	/// <summary>
	/// ���̃N���X���̃V�F�[�_�[�𗘗p����B�f�t�H���g�̓��u�����h
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="alpha_or_add">���̓�ȊO�͐ݒ�ł��Ȃ��B�K�v�Ȃ���</param>
	void BeginDraw(ID3D11DeviceContext* dc, BSMode alpha_or_add = BSMode::Alpha);


	
	/// <summary>
	/// �`����s
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="image_offset"></param>
	/// <param name="image_size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">���W�̊�_</param>
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
	/// �`����s
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">���W�̊�_</param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture, 
		const Vector2& position,
		const Vector2& size, 
		const Vector4& color, 
		TexturePivot texture_pivot);
	/// <summary>
	/// �`����s
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	/// <param name="position"></param>
	/// <param name="texture_pivot">���W�̊�_</param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture,  
		const Vector2& position, 
		TexturePivot texture_pivot);
	/// <summary>
	/// �`����s
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="texture"></param>
	void Draw(
		ID3D11DeviceContext* dc, 
		const Texture& texture);
	/// <summary>
	/// �`����s
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="shader_resource_view"></param>
	/// <param name="texture2d"></param>
	void Draw(
		ID3D11DeviceContext* dc, 
		ID3D11ShaderResourceView*const* shader_resource_view,
		ID3D11Texture2D* texture2d);
	/// <summary>
	/// �`����s
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
	/// �`��I��
	/// </summary>
	void EndDraw(ID3D11DeviceContext* dc) const;


private:

	/// <summary>
	/// ���_���̍X�V
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
	/// ���_���̍X�V
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
	/// �ꖇ�̃e�N�X�`�����܂Ƃ߂ĕ`�悷�鏀��
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="alpha_or_add"></param>
	void BeginDrawBatch(ID3D11DeviceContext* dc, BSMode alpha_or_add = BSMode::Alpha);

	/// <summary>
	/// �܂Ƃ߂ĕ`����s���o�b�t�@�ɏ�������ł���
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="tex_pos"></param>
	/// <param name="tex_size"></param>
	/// <param name="color"></param>
	/// <param name="texture_pivot">���W�̊�_</param>
	void DrawBatch(
		ID3D11DeviceContext* dc,
		const Texture& texture,
		const Vector2& position,
		const Vector2& size,
		const Vector2& tex_pos,
		const Vector2& tex_size,
		const Vector4& color);

	/// <summary>
	/// �܂Ƃ߂ĕ`����s���o�b�t�@�ɏ�������ł���
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
	/// �o�b�t�@�ɏ������񂾃f�[�^�����ۂɕ`�悷��
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

	// ���_���̍X�V
	UpdateVertices(dc, { 0, 0 }, texture_size, { 0, 0 }, texture_size, texture_size.x, texture_size.y, { 1, 1, 1, 1 }, TexturePivot::LeftTop);

	// ���_�o�b�t�@�̐ݒ�
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// �v���~�e�B�u�g�|���W�[�̐ݒ�
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

	// �`��
	dc->Draw(4, 0);
}
