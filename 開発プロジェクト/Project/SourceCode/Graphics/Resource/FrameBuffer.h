#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <array>

#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Camera/Camera.h"
#include "ConstantBuffer.h"

/* --- �錾 ---- */

/// <summary>
/// �����_�[�^�[�Q�b�g�A�[�x�X�e���V���A�r���[�|�[�g�̐ݒ���s���N���X�B
/// �}���`�����_�[�^�[�Q�b�g�ɂ��Ή����Ă���
/// </summary>
class FrameBuffer
{
public:

	FrameBuffer() :
		render_targets(), 
		depth_stencil(), 
		width(), height(), clear_flag(),
		viewports(), frame_buffer_cache() {}

	virtual ~FrameBuffer() = default;

	void Initialize(ID3D11Device* device);
	void EndProcess(ID3D11DeviceContext* dc);

	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɐݒ肵���V�F�[�_���\�[�X�r���[���N���A����B 
	/// </summary>
	void SetNullSRV(ID3D11DeviceContext* dc);
	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɐݒ肵�������_�[�^�[�Q�b�g�r���[���ׂ�null�ɂ���B
	/// ���ݐݒ肵�Ă��郌���_�[�^�[�Q�b�g�A�f�v�X�X�e���V�����N���A����B
	/// </summary>
	void SetNullRTV(ID3D11DeviceContext* dc);

public:

	/// <summary>
	/// �ݒ肵�������_�[�^�[�Q�b�g�A�f�v�X�X�e���V���r���[���f�o�C�X�R���e�L�X�g�ɂ܂Ƃ߂Đݒ肷��B
	/// �ݒ肳��ĂȂ������� nullptr ��ݒ肷�邽�߁A
	/// ���ׂẴ����_�[�^�[�Q�b�g�A�f�v�X�X�e���V���r���[��������������B
	/// </summary>
	void ActivateRenderTargets(ID3D11DeviceContext* dc);
	void ActivateViewPorts(ID3D11DeviceContext* dc, uint32_t num_viewport);

	/// <summary>
	/// ���ݐݒ肵�Ă��郌���_�[�^�[�Q�b�g�A�f�v�X�X�e���V���r���[�̏�����
	/// </summary>
	void ClearCurrentRTVAndDSV(ID3D11DeviceContext* dc, UINT depth_stencil_clear_flag = (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL));

	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
	/// </summary>
	void PrepareRenderTargets(const std::array<std::shared_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT>& render_targets) { this->render_targets = render_targets; }

	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
	/// �����̏��ԂɃC���f�b�N�X�Ƀ����_�[�^�[�Q�b�g��ݒ肷��B
	/// </summary>
	template <class... Args>
	void PrepareRenderTargets(Args const&... args);

	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
	/// </summary>
	void PrepareDepthStencil(const std::shared_ptr<DepthStencil>& depth_stencil) { this->depth_stencil = depth_stencil; }

	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
	/// </summary>
	void PrepareViewports(const std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX>& viewport) { viewports = viewport; }
	
	/// <summary>
	/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
	/// </summary>
	void PrepareViewport(float width, float height) { viewports[0] = { 0.0f, 0.0f, width, height, 0.0f, 1.0f }; }


	const std::shared_ptr<RenderTarget>& GetRenderTarget(uint32_t index) { return render_targets[index]; }

	/// <summary>
	/// �r���[�|�[�g�A�[�x�X�e���V���A�����_�[�^�[�Q�b�g���f�o�C�X�R���e�L�X�g�ɐݒ肷��B
	/// �e���̓o�b�t�@�ɕێ������B
	/// �ݒ肵����A�r���[�|�[�g�A�����_�[�^�[�Q�b�g�͏����������i�J���[��(0)�j
	/// 
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="depth_stencil"></param>
	/// <param name="depth_stencil_clear_flag"></param>
	/// <param name="...args"></param>
	template <class ...Args>
	void Set(
		ID3D11DeviceContext* dc,
		float width, 
		float height, 
		const std::shared_ptr<DepthStencil>& depth_stencil,
		UINT depth_stencil_clear_flag, 
		Args const&... args);

	void ResetCache(ID3D11DeviceContext* dc, bool is_clear_ds);

private:

	// �f�o�C�X�R���e�L�X�g�ɐݒ肷�郌���_�[�^�[�Q�b�g�E�f�v�X�X�e���V���r���[��ێ�����
	// �����𒇉�邱�ƂŁA������x�Ǘ����₷���Ȃ�B
	std::array<std::shared_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> render_targets;
	std::shared_ptr<DepthStencil>	 depth_stencil;
	std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX> viewports;

	float width, height;
	UINT clear_flag;

	class Cache
	{
	public:

		float width, height;
		UINT clear_flag;

		std::array<std::weak_ptr<RenderTarget>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> w_render_targets;
		std::weak_ptr<DepthStencil>	 w_depth_stencil;
		std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX> viewports;
	} frame_buffer_cache;

public:

	/// <summary>
	/// �V�[���萔�o�b�t�@�Ƃ���1�ԃX���b�g�ɑ���f�[�^�����J������ݒ肷��B
	/// </summary>
	/// <param name="camera"></param>
	void PrepareCamera(const std::shared_ptr<Camera>& camera) { frame_camera = camera; }

	/// <summary>
	/// �ݒ肵���J�������ɂ��V�[���萔�o�b�t�@���P�ԃX���b�g�Œ萔�o�b�t�@�Ǘ��N���X�ɑ��M���A
	/// �f�o�C�X�R���e�L�X�g�ɐݒ肷��B
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="camera">nullptr�̏ꍇ�͈ȑO�ɐݒ肵���J�����B���̃N���X�����J�����o�b�t�@���㏑������B</param>
	void SetSceneCB(ID3D11DeviceContext* dc, const std::shared_ptr<Camera>& camera);

	std::shared_ptr<Camera> GetFrameCamera() { return frame_camera; }

private:

	/// <summary>
	/// �V�[���萔�o�b�t�@�\����
	/// </summary>
	struct CBufferScene
	{
		static constexpr uint32_t slot{ 1 };

		Matrix view;			 // �r���[�ϊ��s��
		Matrix projection;		 // �v���W�F�N�V�����ϊ��s��
		Matrix view_inverse;	 // �r���[�t�s��
		Vector4 camera_position; // �J�����̈ʒu
	};
	ConstantBuffer<CBufferScene> scene_cb;
	std::shared_ptr<Camera> frame_camera;


};

/// <summary>
/// �f�o�C�X�R���e�L�X�g�ɑ���ׂ̃o�b�t�@�ɐݒ肵�������ŁA�f�o�C�X�R���e�L�X�g�ɐݒ肳��Ă���킯�ł͂Ȃ�
/// �����̏��ԂɃC���f�b�N�X�Ƀ����_�[�^�[�Q�b�g��ݒ肷��B
/// </summary>
template<class... Args>
inline void FrameBuffer::PrepareRenderTargets(Args const&... args)
{
	_ASSERT_EXPR(sizeof...(args) < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, L"�ݒ�ł��郌���_�[�^�[�Q�b�g�̐��𒴂��Ă��܂��B");

	// �z��̏�����
	for (auto& rt : render_targets) { rt.reset(); }

	for (uint32_t ri = 0;
		 const auto& rt : std::initializer_list<std::shared_ptr<RenderTarget>>{args...})
	{
		render_targets[ri++] = rt;
	}
}

template<class ...Args>
inline void FrameBuffer::Set(
	ID3D11DeviceContext* dc,
	float width, 
	float height, 
	const std::shared_ptr<DepthStencil>& depth_stencil,
	UINT depth_stencil_clear_flag, 
	Args const&...args)
{
	// �L���b�V��
	frame_buffer_cache = {};

	frame_buffer_cache.width = this->width;
	frame_buffer_cache.height = this->height;
	frame_buffer_cache.clear_flag = this->clear_flag;
	frame_buffer_cache.viewports = this->viewports;
	frame_buffer_cache.w_depth_stencil = this->depth_stencil;
	
	for (uint32_t ri = 0; const auto& rt : render_targets)
	{
		if (rt == nullptr) continue;
		frame_buffer_cache.w_render_targets[ri++] = rt;
	}

	this->width = width;
	this->height = height;
	this->clear_flag = depth_stencil_clear_flag;

	PrepareViewport(width, height);
	PrepareDepthStencil(depth_stencil);
	PrepareRenderTargets(args...);

	ClearCurrentRTVAndDSV(dc, depth_stencil_clear_flag);

	ActivateViewPorts(dc, 1);
	ActivateRenderTargets(dc);

}
