#include "FrameBuffer.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/ConstantBufferManager.h"

/* --- ��` ---- */

void FrameBuffer::SetNullRTV(ID3D11DeviceContext* dc)
{
	ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	dc->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, nullptr);

	// ���ݐݒ肵�Ă���r���[�̃N���A
	for (auto& r : render_targets)
	{
		r.reset();
	}


	depth_stencil.reset();
}

void FrameBuffer::Initialize(ID3D11Device* device)
{
	scene_cb.Create(device);

	frame_buffer_cache = {};
}

void FrameBuffer::EndProcess(ID3D11DeviceContext* dc)
{
	SetNullRTV(dc);
	SetNullSRV(dc);
}

void FrameBuffer::SetNullSRV(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, srvs);
	dc->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, srvs);
}

void FrameBuffer::ActivateRenderTargets(ID3D11DeviceContext* dc)
{
	ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	// �C���f�b�N�X�̃f�[�^�� nullptr �Ȃ� nullptr ��z��ɐݒ肷��B
	for (int ri = 0; ri < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++ri)
	{
		rtvs[ri] = (render_targets[ri] != nullptr)
			? render_targets[ri]->GetView().Get()
			: nullptr;
	}

	dc->OMSetRenderTargets(
		D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		rtvs,
		depth_stencil ? depth_stencil->GetView().Get() : nullptr);
}

void FrameBuffer::ActivateViewPorts(ID3D11DeviceContext* dc, uint32_t num_viewport)
{
	dc->RSSetViewports(num_viewport, viewports.data());


}

void FrameBuffer::ClearCurrentRTVAndDSV(ID3D11DeviceContext* dc, UINT depth_stencil_clear_flag)
{
	for (auto& rt : render_targets)
	{
		if(rt) rt->Clear(dc);
	}

	if(depth_stencil) depth_stencil->Clear(dc, depth_stencil_clear_flag);
}

void FrameBuffer::ResetCache(ID3D11DeviceContext* dc, bool is_clear_ds)
{
	this->viewports = frame_buffer_cache.viewports;
	PrepareDepthStencil(frame_buffer_cache.w_depth_stencil.lock());

	for (size_t ri = 0ULL; ri < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++ri)
	{
		if (auto rt = frame_buffer_cache.w_render_targets[ri].lock())
		{
			render_targets[ri] = rt;
		}
	}

	if (is_clear_ds)
	{
		ClearCurrentRTVAndDSV(dc, frame_buffer_cache.clear_flag);
	}

	ActivateViewPorts(dc, 1);
	ActivateRenderTargets(dc);

}

void FrameBuffer::SetSceneCB(
	ID3D11DeviceContext* dc, 
	const std::shared_ptr<Camera>& camera)
{
	// �J�����������ɐݒ肵���ꍇ�㏑������
	if (camera)
	{
		PrepareCamera(camera);
	}

	// �V�[���萔�o�b�t�@�̍X�V
	{
		CBufferScene cb{};

		cb.view = frame_camera->GetView();
		cb.view_inverse = cb.view.Inverse();
		cb.projection = frame_camera->GetProjection();
		cb.camera_position = &frame_camera->GetEye().x;

		scene_cb.UpdateSubresouce(dc, cb);
	}

	// �萔�o�b�t�@�Ǘ��N���X�Ƀf�[�^�𑗐M���A�f�o�C�X�R���e�L�X�g�ɐݒ肷��B
	Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(
		dc,
		CBufferScene::slot,
		&scene_cb);
}
