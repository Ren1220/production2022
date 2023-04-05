#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <filesystem>
#include <d3d11.h>
#include <wrl.h>
#include <string>

/// <summary>
/// �����̉摜�̃A�j���[�V������`��
/// </summary>
class TextureAnimation
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// <para>�t�@�C���p�X�z�����͂���</para>
	/// </summary>
	/// <param name="paths"></param>
	TextureAnimation(const std::vector<std::wstring>& paths);

	/// <summary>
	/// �R���X�g���N�^
	/// <para>�t�H���_���̃f�[�^��ǂݍ���(�\�[�g��filesystem�ɂ���čs����)</para>
	/// <para>�~ 1.png, 2.png ... 11.png, 12.png => 1.png, 11.png, 12.png, 2.png... �ƂȂ�</para>
	/// <para>�Z 01.png, 02.png ... 11.png, 12.png => 01.png, 02.png ... 11.png, 12.png </para>
	/// </summary>
	TextureAnimation(const std::string folder_path);
	~TextureAnimation();

private:

	struct Textures
	{
	public:

		Textures(const std::vector<std::wstring>& paths);
		~Textures() = default;

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texcoord;
		};

		struct TextureResource
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
			D3D11_TEXTURE2D_DESC texture2d_desc;
		};

	public:

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;


		std::vector<std::unique_ptr<TextureResource>> texture_resources;

		std::vector<Vertex> vertices;
		const size_t max_vertices;

	public:

		/// <summary>
		/// �w��̃e�N�X�`���C���f�b�N�X�̕`�������
		/// </summary>
		/// <param name="index"></param>
		void Render(ID3D11DeviceContext* dc, const size_t index, float dx, float dy, float dw, float dh,
			float tx, float ty, float tw, float th,
			float angle, float r, float g, float b, float a);

		/// <summary>
		/// �w��̃e�N�X�`���C���f�b�N�X�̕`�������
		/// </summary>
		/// <param name="index"></param>
		void Render(ID3D11DeviceContext* dc, const size_t index, float dx, float dy, float dw, float dh);

	};

private:

	// �摜�f�[�^���i�[����z��
	std::unique_ptr<Textures> textures;

	/// <summary>
	/// �T���v���[�X�e�[�g
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	/// <summary>
	/// �u�����h�X�e�[�g
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

	// �\������摜�̍����ԍ�
	size_t texture_index{ 0 };

	// �ꖇ�̉摜��\��������t���[����
	float animation_frame { 1.0f / 60 };

	// �A�j���[�V�����t���[�����Ǘ�����^�C�}�[
	float animation_timer{ 0.0f };

public:

	void SetAnimationFrame(float set_animation_frame) { animation_frame = set_animation_frame; }

	/// <summary>
	/// �C�ӂ̃��[�v���Ԃɂ������̃A�j���[�V�����̃t���[�������v�Z����
	/// </summary>
	float CalculateAnimationFramePerTime(float loop_time);

	/// <summary>
	/// �e�N�X�`���̉������擾����(�e�N�X�`���̃T�C�Y�͂��ׂē����T�C�Y�Ƃ݂Ȃ�)
	/// </summary>
	/// <returns></returns>
	float GetTextureWidth() const;

	/// <summary>
	/// �e�N�X�`���̏c�����擾����(�e�N�X�`���̃T�C�Y�͂��ׂē����T�C�Y�Ƃ݂Ȃ�)
	/// </summary>
	/// <returns></returns>
	float GetTextureHeight() const;

public:

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time"></param>
	void Update(float elapsed_time);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Render(ID3D11DeviceContext* dc,
		float dx, float dy, float dw, float dh,
		float tx, float ty, float tw, float th,
		float angle, float r, float g, float b, float a);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Render(ID3D11DeviceContext* dc, float dx, float dy, float dw, float dh);
};