#pragma once

/* --- �w�b�_�[�t�@�C���̃C���N���[�h ---- */

#include "Shader.h"

#include "Arithmetic/Arithmetic.h"

#include "../Resource/Texture/Texture.h"
#include "../Resource/ConstantBuffer.h"

/* --- �N���X��` ---- */

/// <summary>
/// t0 �Ƀe�N�X�`����ݒ�Bb4 �ɒ萔�o�b�t�@��ݒ�B
/// </summary>
class Sprite2DShader : public Shader
{
public:


	Sprite2DShader()
		: Shader() {};
	~Sprite2DShader() override = default;

	/// <summary>
	/// ���̓��C�A�E�g�̏��������s��
	/// </summary>
	/// <param name="device"></param>
	void Create(ID3D11Device* device) override;


	// �e�N�X�`����null�̎��̓_�~�[�e�N�X�`�����g�p����
	void Activate(ID3D11DeviceContext* dc) override;
	// �e�N�X�`����null�̎��̓_�~�[�e�N�X�`�����g�p����
	void Activate(ID3D11DeviceContext* dc, const Vector4& material_color);
	// �e�N�X�`����null�̎��̓_�~�[�e�N�X�`�����g�p����
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

