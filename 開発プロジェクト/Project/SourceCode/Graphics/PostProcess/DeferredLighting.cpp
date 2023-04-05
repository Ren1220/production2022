#include "DeferredLighting.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "imgui.h"

#include "Include/misc.h"

#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Texture/TextureRenderer.h"

/* --- ��` ---- */

void DeferredLighting::Initialize(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr{ S_OK };

	this->width = width;
	this->height = height;

	// �W�I���g���o�b�t�@�̍쐬
	geometry_buffer.Create(device, width, height);

	// ���C�g�o�b�t�@HDR�ɑΉ������Ă���
	lighting_rt = std::make_shared<RenderTarget>();
	lighting_rt->Create(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// �����萔�o�b�t�@�̍쐬
	light_cb.Create(device);

	// �V�F�[�_�[
	{
		lighting_shader.CreateVSFromCSO(device, "Shader/Sprite2DVS.cso");
		lighting_shader.CreatePSFromCSO(device, "Shader/DeferredLightingPS.cso");
		lighting_shader.CreateBlendState(device, BSMode::Alpha);
		lighting_shader.CreateDepthStencilState(device, DSMode::Depth_Stencil);
		lighting_shader.CreateRasterizerState(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false);
		lighting_shader.CreateSamplerState(device, 0, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 1, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 2, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
		lighting_shader.CreateSamplerState(device, 3, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER);
	}

	// �S���ʃV���h�E�e�N�X�`���̕i����1024*1024�ōs��
	shadow_caster.Create(device, 1024, 1024);
}

void DeferredLighting::EndProcess(ID3D11DeviceContext* dc)
{
}

void DeferredLighting::Update()
{
	// �Q�Ɛ悪���݂��Ȃ��ꍇ�͔z�񂩂珜�O
	auto remove_itr{ std::ranges::remove_if(w_lights, [](const auto& w_light) {return w_light.expired(); }) };
	// erase���Ȃ��ƃf�[�^�͔z�񂩂珜�O����Ă��Ȃ�
	w_lights.erase(remove_itr.begin(), w_lights.end());
}

void DeferredLighting::ActivateGeometryBuffer(ID3D11DeviceContext* dc)
{
	geometry_buffer.ActivateGeometry(dc);
}

void DeferredLighting::ActivateBufferForEffekseer(ID3D11DeviceContext* dc)
{
	Graphics::GetInstance().GetFrameBuffer()->Set(dc,
		static_cast<float>(width),
		static_cast<float>(height),
		geometry_buffer.depth_stencil,
		0U,							 // �[�x�X�e���V���r���[�̏������͍s��Ȃ�
		geometry_buffer.emmisive_rt);// ���Ȕ���			    
}

void DeferredLighting::ShadowCast(
	ID3D11DeviceContext* dc)
{
	// �������݊J�n
	shadow_caster.Activate(dc);

	for(size_t li = 0; li < w_lights.size(); ++li)
	{
		auto light{ w_lights[li].lock() };

		if (li > shadow_caster.GetMaxLightCount()) break; // �Ƃ肠�������C�g�͂P�O�܂�

		// �_�����̂�(�����p�����[�^���O�ȏ�̌�����_�����Ƃ݂Ȃ�)
		if (light->light_parameter.w >= 0.1f)
		{
			constexpr float near_z{ 0.1f }; // �V���h�E�C���O�p�̃v���W�F�N�V�����s���near_z��������

			shadow_caster.ClearRTV(dc);

			Vector3 eye{ &light->light_parameter.x };
			// ��������^���Ē萔�o�b�t�@���X�V
			shadow_caster.UpdateCBuffer(dc, eye, near_z, light->light_parameter.w);
			shadow_caster.SetCBuffer(dc, CBufferLight::SLOT);
			// �[�x������������
			shadow_caster.RenderModel(dc);
			// �������񂾐[�x�����L���[�u�}�b�v�z��̑Ή������C���f�b�N�X�ɃR�s�[����
			shadow_caster.WriteTextureCubeArray(dc, static_cast<uint32_t>(li));
		}
	}

	// �������ݏI��
	shadow_caster.Deactivate(dc);
}

void DeferredLighting::Lighting(ID3D11DeviceContext* dc)
{
	auto& frame_buffer{ Graphics::GetInstance().GetFrameBuffer() };
	auto& constant_buffer_manager{ Graphics::GetInstance().GetConstantBufferManager() };

	// ���C�g�o�b�t�@��ݒ肷��
	frame_buffer->Set(
		dc,
		static_cast<float>(width),
		static_cast<float>(height),
		nullptr,
		0,
		lighting_rt);

	ID3D11ShaderResourceView* srvs[]
	{
		geometry_buffer.color_rt->GetTexture().GetView().Get(),				 // 0 : �F
		geometry_buffer.roughness_metallic_rt->GetTexture().GetView().Get(), // 1 : ���t�l�X�E���^���b�N
		geometry_buffer.emmisive_rt->GetTexture().GetView().Get(),			 // 2 : ���Ȕ���
		geometry_buffer.occlusion_rt->GetTexture().GetView().Get(),			 // 3 : �A�e
		geometry_buffer.normal_rt->GetTexture().GetView().Get(),			 // 4 : �@��
		geometry_buffer.world_position_rt->GetTexture().GetView().Get(),	 // 5 : 3D���W
		shadow_caster.GetShadowCastTextureCubeArray().srv.Get()
	};

	// �o�b�t�@�Ƀ��C�e�B���O���ʂ���������
	
	{
		dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �`��J�n
		Graphics::GetInstance().GetTextureRenderer()->BeginDraw(dc, &lighting_shader);

		// �萔�o�b�t�@�̍X�V
		{
			CBufferLight cb{};

			// �ݒ肵�Ă��郉�C�g�̏����o�b�t�@�ɏ�������ł���
			for (size_t li = 0; li < w_lights.size(); ++li)
			{
				if (li > shadow_caster.GetMaxLightCount()) break; // �Ƃ肠�������C�g�͂P�O�܂�

				memcpy_s(&cb.light[li], sizeof(Light), w_lights[li].lock().get(), sizeof(Light));
			}

			light_cb.UpdateSubresouce(dc, cb);
			constant_buffer_manager->SetBuffer(dc, CBufferLight::SLOT, &light_cb);
		}


		// �`����s
		Graphics::GetInstance().GetTextureRenderer()->Draw(
			dc,
			srvs,
			_countof(srvs),
			static_cast<float>(width),
			static_cast<float>(height));

		Graphics::GetInstance().GetTextureRenderer()->EndDraw(dc);
	}
}

void DeferredLighting::ImGuiRender()
{
	ImGui::Begin("Deferred");

	geometry_buffer.ImGuiRender();

	ImGui::End();

	shadow_caster.ImGuiRender();
}

void DeferredLighting::RegisterLight(const std::shared_ptr<Light>& light_data)
{
	_ASSERT_EXPR(static_cast<uint32_t>(w_lights.size()) < shadow_caster.GetMaxLightCount(), L"�ݒ肵���V���h�E�C���O���s���_�����̐��𒴂��Ă��܂��B");
	w_lights.emplace_back(light_data);
}
