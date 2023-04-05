#include "TextureAnimation.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "General/AtaiEngine.h"
#include "Include/misc.h"

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/ResourceFileloader.h"

/* --- ��` ---- */

TextureAnimation::Textures::Textures(const std::vector<std::wstring>& paths)
	:max_vertices(6 * paths.size())
{
	ID3D11Device* device{ MySystem.GetDevice() };

	HRESULT hr{ S_OK };

	// ���_�o�b�t�@�̍쐬
	{
		std::unique_ptr<Vertex[]> vertices{ std::make_unique<Vertex[]>(max_vertices) };

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * max_vertices);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = vertices.get();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&desc, &data, vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// �C���v�b�g���C�A�E�g�I�u�W�F�N�g�̍쐬
	{
		D3D11_INPUT_ELEMENT_DESC desc[]
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		// �V�F�[�_�[�I�u�W�F�N�g�̍쐬
		ResourceFileLoader::CreateVSfromCSO(device, "./Shader/Sprite2DVS.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), desc, ARRAYSIZE(desc));
		ResourceFileLoader::CreatePSfromCSO(device, "./Shader/Sprite2DPS.cso", pixel_shader.ReleaseAndGetAddressOf());

	}

	// ���\�[�X�z��̃T�C�Y�����߂�
	//texture_resources.resize(paths.size());

	// �e�N�X�`���̓ǂݍ���
	for(size_t i = 0; i < paths.size(); ++i)
	{
		std::unique_ptr<TextureResource> resource{ std::make_unique<TextureResource>() };

		// �e�N�X�`����ǂݍ���
		hr = ResourceFileLoader::LoadTexture(device, paths[i].c_str(), resource->shader_resource_view.ReleaseAndGetAddressOf(), &resource->texture2d_desc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �ǂݍ��񂾃f�[�^��z��Ɋi�[����
		texture_resources.emplace_back(std::move(resource));
	}

}

void TextureAnimation::Textures::Render(ID3D11DeviceContext* dc,
	const size_t index,
	float dx, float dy, float dw, float dh,
	float tx, float ty, float tw, float th,
	float angle, float r, float g, float b, float a)
{
	HRESULT hr{ S_OK };

	// ���_�������Z�b�g����
	vertices.clear();

	// �r���[�|�[�g�̐ݒ�T�C�Y���擾����
	D3D11_VIEWPORT vp;
	UINT num_vp{ 1 };
	dc->RSGetViewports(&num_vp, &vp);

	// �����o�֐��̈��������`�̒��_�̈ʒu���v�Z����
	float dx0{ dx }, dy0{ dy }, tx0{ tx }, ty0{ ty };
	float dx1{ dx + dw }, dy1{ dy }, tx1{ tx + tw }, ty1{ ty };
	float dx2{ dx }, dy2{ dy + dh }, tx2{ tx }, ty2{ ty + th };
	float dx3{ dx + dw }, dy3{ dy + dh }, tx3{ tx + tw }, ty3{ ty + th };

	// �S���_�̉�]������̈ʒu���v�Z����

	// ��]�̒��S����`�̒��S�Ƃ���
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;


	/// <summary>
	/// �w�x�̉�]�̌v�Z
	/// </summary>
	/// <param name="x">�w���W</param>
	/// <param name="y">�x���W</param>
	/// <param name="cx">���S�̈ʒu</param>
	/// <param name="cy">���S�̈ʒu</param>
	/// <param name="angle">��]��</param>
	auto Rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};

	Rotate(dx0, dy0, cx, cy, angle);
	Rotate(dx1, dy1, cx, cy, angle);
	Rotate(dx2, dy2, cx, cy, angle);
	Rotate(dx3, dy3, cx, cy, angle);

	// �X�N���[�����W�n����m�c�b�ւ̍��W�ϊ����s��
	dx0 = 2.0f * dx0 / vp.Width - 1.0f;
	dy0 = 1.0f - 2.0f * dy0 / vp.Height;
	dx1 = 2.0f * dx1 / vp.Width - 1.0f;
	dy1 = 1.0f - 2.0f * dy1 / vp.Height;
	dx2 = 2.0f * dx2 / vp.Width - 1.0f;
	dy2 = 1.0f - 2.0f * dy2 / vp.Height;
	dx3 = 2.0f * dx3 / vp.Width - 1.0f;
	dy3 = 1.0f - 2.0f * dy3 / vp.Height;

	// �t�u���W���v�Z����
	D3D11_TEXTURE2D_DESC texture2d_desc{ texture_resources[index]->texture2d_desc };

	float u0{ tx / texture2d_desc.Width };
	float v0{ ty / texture2d_desc.Height };
	float u1{ (tx + tw) / texture2d_desc.Width };
	float v1{ (ty + th) / texture2d_desc.Height };

	// ���_����ݒ肷��
	vertices.push_back(Textures::Vertex( {dx0, dy0, 0}, {r, g, b, a}, {u0, v0} ));
	vertices.push_back(Textures::Vertex( {dx1, dy1, 0}, {r, g, b, a}, {u1, v0} ));
	vertices.push_back(Textures::Vertex( {dx2, dy2, 0}, {r, g, b, a}, {u0, v1} ));
	vertices.push_back(Textures::Vertex( {dx2, dy2, 0}, {r, g, b, a}, {u0, v1} ));
	vertices.push_back(Textures::Vertex( {dx1, dy1, 0}, {r, g, b, a}, {u1, v0} ));
	vertices.push_back(Textures::Vertex( {dx3, dy3, 0}, {r, g, b, a}, {u1, v1} ));

	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	{
		D3D11_MAPPED_SUBRESOURCE map{};
		hr = dc->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		size_t vertex_conut{ vertices.size() };
		_ASSERT_EXPR(max_vertices >= vertex_conut, "Buffer Overflow");

		Vertex* data{ reinterpret_cast<Vertex*>(map.pData) };

		if (data != nullptr)
		{
			const Vertex* p{ vertices.data() };
			memcpy_s(data, max_vertices * sizeof(Vertex), p, vertex_conut * sizeof(Vertex));
		}

		dc->Unmap(vertex_buffer.Get(), 0);
	}

	ID3D11ShaderResourceView* srv{ texture_resources[index]->shader_resource_view.Get() };

	UINT stride{ sizeof(Vertex) }, offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	dc->PSSetShaderResources(0, 1, &srv);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(input_layout.Get());
	dc->VSSetShader(vertex_shader.Get(), nullptr, 0);
	dc->PSSetShader(pixel_shader.Get(), nullptr, 0);

	dc->Draw(static_cast<UINT>(vertices.size()), 0);

}

void TextureAnimation::Textures::Render(ID3D11DeviceContext* dc, const size_t index, float dx, float dy, float dw, float dh)
{
	float w{ static_cast<float>(texture_resources[index]->texture2d_desc.Width) };
	float h{ static_cast<float>(texture_resources[index]->texture2d_desc.Height) };
	Render(dc, index, dx, dy, dw, dh, 0.0f, 0.0f, w, h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

TextureAnimation::TextureAnimation(const std::vector<std::wstring>& paths)
{
	textures = std::make_unique<Textures>(paths);

	DXHelper::CreateBlendState(MySystem.GetDevice(), blend_state.ReleaseAndGetAddressOf(), BSMode::Alpha);
	DXHelper::CreateSamplerState(MySystem.GetDevice(), sampler_state.ReleaseAndGetAddressOf(), D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP);
}

TextureAnimation::TextureAnimation(const std::string folder_path)
{
	std::vector<std::wstring> paths;

	for (const auto& file : std::filesystem::directory_iterator(folder_path))
	{
		paths.push_back(file.path().wstring());
	}

	textures = std::make_unique<Textures>(paths);

	DXHelper::CreateBlendState(MySystem.GetDevice(), blend_state.ReleaseAndGetAddressOf(), BSMode::Alpha);
	DXHelper::CreateSamplerState(MySystem.GetDevice(), sampler_state.ReleaseAndGetAddressOf(), D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP);
}


TextureAnimation::~TextureAnimation()
{
}

float TextureAnimation::CalculateAnimationFramePerTime(float loop_time)
{
	return loop_time / textures->texture_resources.size();
}

float TextureAnimation::GetTextureWidth() const
{
	return static_cast<float>(textures->texture_resources[0]->texture2d_desc.Width);
}

float TextureAnimation::GetTextureHeight() const
{
	return static_cast<float>(textures->texture_resources[0]->texture2d_desc.Height);
}

void TextureAnimation::Update(float elapsed_time)
{
	// �\������摜���X�V����
	if (animation_timer > animation_frame)
	{
		// �����ԍ�������炷
		texture_index++;

		// �z������[�v������
		if (texture_index >= textures->texture_resources.size())
		{
			texture_index = 0;
		}

		animation_timer = 0.0f;
	}

	// �^�C�}�[�����Z����
	animation_timer += elapsed_time;
}

void TextureAnimation::Render(ID3D11DeviceContext* dc,
	float dx, float dy, float dw, float dh,
	float tx, float ty, float tw, float th,
	float angle, float r, float g, float b, float a)
{
	dc->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	dc->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);

	textures->Render(dc, texture_index, dx, dy, dw, dh, tx, ty, tw, th, angle, r, g, b, a);
}

void TextureAnimation::Render(ID3D11DeviceContext* dc, float dx, float dy, float dw, float dh)
{
	dc->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	dc->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);

	textures->Render(dc, texture_index, dx, dy, dw, dh);
}
