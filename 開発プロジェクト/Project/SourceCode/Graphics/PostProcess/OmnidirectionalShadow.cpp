#include "OmnidirectionalShadow.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"

#include "Include/misc.h"
#include "Game/General/GameSystem.h"
#include "Game/Stage/StageManager.h"
#include "Game/Actor/ActorManager.h"
#include "Game/Actor/Actor.h"

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/PixelBuffer.h"

/* --- ��` ---- */

OmnidirectionalShadow::OmnidirectionalShadow()
	: width(), height(), max_light_count()
{
#ifdef _DEBUG
	is_shadow_cast = true;
#endif // DEBUG
}

OmnidirectionalShadow::~OmnidirectionalShadow()
{
}

void OmnidirectionalShadow::Create(
	ID3D11Device* device, 
	uint32_t tex_w,
	uint32_t tex_h, 
	uint32_t max_light_count,
	DXGI_FORMAT srv_format)
{
	HRESULT hr{ S_OK };

	this->max_light_count = max_light_count;
	width = tex_w;
	height = tex_h;

	// �����_�[�^�[�Q�b�g�̍쐬
	CreateRenderTarget(device, srv_format);

	// �V���h�E���������ރL���[�u�}�b�v�̍쐬
	CreateTextureCubeArray(device, max_light_count, srv_format);

	// �萔�o�b�t�@
	cube_map_view_cb.Create(device);

	// �ʏ�̃��f���̃V���h�E�L���X�g�p�̃V�F�[�_�[
	{
		shadow_cast_shader.CreateVSFromCSO(device, "Shader/ShadowCastCubeMapVS.cso");
		shadow_cast_shader.CreateGSFromCSO(device, "Shader/ShadowCastCubeMapGS.cso");
		shadow_cast_shader.CreatePSFromCSO(device, "Shader/ShadowCastCubeMapPS.cso");
		shadow_cast_shader.CreateBlendState(device, BSMode::Darken); // BlendMode��Darken�ɂ��邱�Ƃōł���O�ɂ��郂�f���̐[�x�𗘗p����
		shadow_cast_shader.CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_NONE);
	}

	// �X�e�[�W�̃��f���̃V���h�E�L���X�g�p�̃V�F�[�_�[
	// �X�e�[�W�ɂ̓C���X�^���X�`��𗘗p���Ă��邽�ߌʂŃV�F�[�_�[���쐬����B
	{
		shadow_cast_shader_instanced.CreateVSFromCSO(device, "Shader/ShadowCastCubeMapInstancedVS.cso");
		// �f�B�X�v���[�X�����g�}�b�v�𗘗p�����ꍇ�̒n�`�̃V���h�E�C���O�B
		// ���݃f�B�X�v���[�X�����g���Ƀ��b�V�����m�̐ڍ������Ɍ��Ԃ��o���Ă��܂����ߖ��g�p
		//shadow_cast_shader_instanced.CreateHSFromCSO(device, "Shader/ShadowCastCubeMapInstancedHS.cso");
		//shadow_cast_shader_instanced.CreateDSFromCSO(device, "Shader/ShadowCastCubeMapInstancedDS.cso");
		shadow_cast_shader_instanced.CreateGSFromCSO(device, "Shader/ShadowCastCubeMapGS.cso");
		shadow_cast_shader_instanced.CreatePSFromCSO(device, "Shader/ShadowCastCubeMapPS.cso");
		shadow_cast_shader_instanced.CreateBlendState(device, BSMode::Darken);		// BlendMode��Darken�ɂ��邱�Ƃōł���O�ɂ��郂�f���̐[�x�𗘗p����
		//shadow_cast_shader_instanced.CreateSamplerState(device, 0, D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		//shadow_cast_shader_instanced.CreateSamplerState(device, 1, D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
		//shadow_cast_shader_instanced.CreateSamplerState(device, 2, D3D11_FILTER_MINIMUM_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
	}

	// �ۉe
	{
		circle_model = std::make_shared<GLTFStaticMesh>(device, "Data/Model/GLTF/Shadow/circle.gltf");
	}

}

void OmnidirectionalShadow::Activate(ID3D11DeviceContext* dc)
{
	// �萔�o�b�t�@�̐ݒ�
	SetCBuffer(dc, CBufferCubeMapView::SLOT);

	// �t���[���o�b�t�@�̐ݒ�
	Graphics::GetInstance().GetFrameBuffer()->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		render_target);
}

void OmnidirectionalShadow::ClearRTV(ID3D11DeviceContext* dc)
{
	float color[4]{ FLT_MAX, 0, 0, 1.0f };
	render_target->Clear(dc, color);
}

void OmnidirectionalShadow::WriteTextureCubeArray(
	ID3D11DeviceContext* dc, 
	int32_t index)
{
	D3D11_BOX box{};
	box.left = 0;
	box.right = width;		// �L���[�u�}�b�v�ʂ̃T�C�Y
	box.top = 0;
	box.bottom = height;    // �L���[�u�}�b�v�ʂ̃T�C�Y
	box.front = 0;
	box.back = 1;

	// �����_�[�^�[�Q�b�g�ɏ������܂ꂽ����Ή�����C���f�b�N�X�̃L���[�u�}�b�v�z��ɃR�s�[����
	for (int i = 0; i < 6; ++i)
	{
		dc->CopySubresourceRegion(
			shadow_cast_texture_cube_array.texture.Get(), // �L���[�u�}�b�v�z��
			(index * 6) + i,							  // �������ރe�N�X�`���B�L���[�u�}�b�v�e�N�X�`���͂U���̃e�N�X�`���łP�P��
			0, 0, 0,
			render_target->GetTexture().GetResource().Get(),
			i,											  // �L���[�u�}�b�v�̂ǂ̖ʂ̃e�N�X�`���� 
			&box);
	}

}

void OmnidirectionalShadow::RenderModel(ID3D11DeviceContext* dc)
{
	auto& actor_manager{ GameSys.GetActorManager() };

#ifdef _DEBUG
	if (is_shadow_cast)
#endif // _DEBUG
	{
		// �n�`�̃V���h�E�̏�������
		GameSys.GetStageManager()->Render(dc, &shadow_cast_shader_instanced);

		// �V���h�E��`�悷��O���[�v�̃A�N�^�̃V���h�E�̏�������
		for (const auto& a : actor_manager->FindActors({ "Enemy", "Item" }))
		{
			a->Render(dc, &shadow_cast_shader);
		}

		// �ۉe
		//{
		//	for (const auto& a : actor_manager->FindActors("Player"))
		//	{
		//		RenderCircle(dc, a->GetWorldPosition(), 0.75f);
		//	}

		//	for (const auto& a : actor_manager->FindActors("Boss"))
		//	{
		//		RenderCircle(dc, a->GetWorldPosition(), 2.0f);
		//	}
		//}
	}
}

void OmnidirectionalShadow::Deactivate(ID3D11DeviceContext* dc)
{
#ifdef _DEBUG
	// ImGui�ŃL���[�u�}�b�v���Q�Ƃ���ׂɊe�ʂ̃e�N�X�`�����R�s�[����B
	D3D11_BOX box{};
	box.left = 0;
	box.right = width;		// �L���[�u�}�b�v�ʂ̃T�C�Y
	box.top = 0;
	box.bottom = height;    // �L���[�u�}�b�v�ʂ̃T�C�Y
	box.front = 0;
	box.back = 1;

	for (int i = 0; i < 6; ++i)
	{
		// ImGui�ɐ擪�̃L���[�u�}�b�v�̏����݌��ʂ��R�s�[����
		dc->CopySubresourceRegion(
			copy_textures[i].Get(),
			0,
			0,
			0,
			0,
			shadow_cast_texture_cube_array.texture.Get(),
			i,
			&box);
	}
#endif // _DEBUG
}

void OmnidirectionalShadow::ImGuiRender()
{
#ifdef _DEBUG
	ImGui::Begin("OmnidirectionalShadow");

	ImGui::Checkbox("ShadowCast", &is_shadow_cast);

	ImGui::Text("+Z");
	ImGui::Image(copy_srvs[4].Get(), { 128, 128 });
	ImGui::Text("+X <----> -X");
	ImGui::Image(copy_srvs[0].Get(), { 128, 128 });
	ImGui::SameLine();
	ImGui::Image(copy_srvs[1].Get(), { 128, 128 });
	ImGui::Text("-Z");
	ImGui::Image(copy_srvs[5].Get(), { 128, 128 });
	ImGui::Text("+Y <----> -Y");
	ImGui::Image(copy_srvs[2].Get(), { 128, 128 });
	ImGui::SameLine();
	ImGui::Image(copy_srvs[3].Get(), { 128, 128 });

	ImGui::End();
#endif // _DEBUG
}

void OmnidirectionalShadow::SetCBuffer(ID3D11DeviceContext* dc, uint32_t slot)
{
	Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, slot, &cube_map_view_cb);
}

void OmnidirectionalShadow::CreateRenderTarget(
	ID3D11Device* device, 
	DXGI_FORMAT srv_format)
{
	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Format = PixelBuffer::GetBaseFormat(srv_format);
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 6;															// �L���[�u�}�b�v�ݒ�
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;							// �L���[�u�}�b�v�ݒ�
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc{};
	srvdesc.Format = srv_format;
	srvdesc.TextureCube.MipLevels = 1;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;					// �L���[�u�}�b�v�ݒ�

	D3D11_RENDER_TARGET_VIEW_DESC rtvdesc{};
	rtvdesc.Format = srv_format;
	rtvdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvdesc.Texture2DArray.ArraySize = 6;										// �L���[�u�}�b�v�ݒ�
	rtvdesc.Texture2DArray.FirstArraySlice = 0;
	rtvdesc.Texture2DArray.MipSlice = 0;

	// ��L�̏��Ń����_�[�^�[�Q�b�g�쐬
	render_target = std::make_shared<RenderTarget>();
	render_target->Create(device, &desc, nullptr, &srvdesc, &rtvdesc);

#ifdef _DEBUG
	// ImGui�ŏ������񂾃L���[�u�}�b�v�̃e�N�X�`�����Q�Ƃ��邽�߂̃o�b�t�@
	for (int i = 0; i < 6; ++i)
	{
		desc.Format = srv_format;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;

		hr = device->CreateTexture2D(&desc, nullptr, copy_textures[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(copy_textures[i].Get(), &srvdesc, copy_srvs[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
#endif // _DEBUG
}

void OmnidirectionalShadow::CreateTextureCubeArray(
	ID3D11Device* device, 
	uint32_t max_array, 
	DXGI_FORMAT srv_format)
{
	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;										// ��ʂ̃T�C�Y
	desc.Height = height;									// ��ʂ̃T�C�Y
	desc.Format = PixelBuffer::GetBaseFormat(srv_format);
	desc.ArraySize = 6 * max_array;							// �L���[�u�}�b�v�̏ꍇ�͂U���łP�P��
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = 1;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;		// �L���[�u�}�b�v�̐ݒ�

	hr = device->CreateTexture2D(
		&desc, 
		nullptr, 
		shadow_cast_texture_cube_array.texture.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = srv_format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY; // �L���[�u�}�b�v�̐ݒ�
	srv_desc.TextureCubeArray.First2DArrayFace = 0;
	srv_desc.TextureCubeArray.MipLevels = desc.MipLevels;
	srv_desc.TextureCubeArray.MostDetailedMip = 0;
	srv_desc.TextureCubeArray.NumCubes = max_array;                // �L���[�u�}�b�v�̐�

	hr = device->CreateShaderResourceView(
		shadow_cast_texture_cube_array.texture.Get(), 
		&srv_desc, 
		shadow_cast_texture_cube_array.srv.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void OmnidirectionalShadow::RenderCircle(
	ID3D11DeviceContext* dc, 
	const Vector3& position, 
	float radius)
{
	Vector3 p{ position };
	p.y = 0.1f;
	Matrix transform{ Matrix::CreateTransform(p, Vector3(), {radius, 1.0f, radius})};
	circle_model->SetWorldTransform(transform);
	circle_model->Render(dc, &shadow_cast_shader);
}

void OmnidirectionalShadow::UpdateCBuffer(
	ID3D11DeviceContext* dc,
	const Vector3& light_pos,
	float near_z,
	float far_z)
{
	CBufferCubeMapView cb{};

	Vector3 eye = light_pos;
	cb.eye = Vector4(eye, 1.0f);

	// ����p��90�x�A�A�X�y�N�g���1�Ńv���W�F�N�V�����s����쐬����
	cb.projection = Matrix::CreateProjection(90.0f, 1.0f, near_z, far_z);

	// ���_�𒆐S�Ƃ��Ċe���̐����̕������������r���[�s��
	cb.view[0] = Matrix::CreateLookAtLH(eye, eye + Vector3(+1, 0, 0), Vector3(0, 1, 0));
	cb.view[1] = Matrix::CreateLookAtLH(eye, eye + Vector3(-1, 0, 0), Vector3(0, 1, 0));
	cb.view[2] = Matrix::CreateLookAtLH(eye, eye + Vector3(0, +1, 0), Vector3(0, 0, -1));
	cb.view[3] = Matrix::CreateLookAtLH(eye, eye + Vector3(0, -1, 0), Vector3(0, 0, 1));
	cb.view[4] = Matrix::CreateLookAtLH(eye, eye + Vector3(0, 0, +1), Vector3(0, 1, 0));
	cb.view[5] = Matrix::CreateLookAtLH(eye, eye + Vector3(0, 0, -1), Vector3(0, 1, 0));

	// �萔�o�b�t�@�̍X�V
	cube_map_view_cb.UpdateSubresouce(dc, cb);
}
