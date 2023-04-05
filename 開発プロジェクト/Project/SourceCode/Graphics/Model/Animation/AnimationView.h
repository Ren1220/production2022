#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <d3d11.h>
#include <wrl.h>

#include <memory>

#include "Game/Animation/Animation.h"

#include "../../Resource/RenderTarget.h"
#include "../../Resource/DepthStencil.h"
#include "../GLTFModel.h"
#include "../GLTFSkinnedMesh.h"


/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class AnimationView 
{
public:

	AnimationView()
		: viewport({}), render_target(), depth_stencil(),
		current_animation_registerd_names(), animation_timer(), animation_loop_flag(false) {};
	~AnimationView() = default;

	void Initialize(ID3D11Device* device, float view_width, float view_height);

	/// <summary>
	/// �A�j���[�V�����r���[���f���̃A�j���[�V�����̍X�V
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateCurrentAnimationData(float elapsed_time);

	/// <summary>
	/// �A�j���[�V�����r���[�`�揈��
	/// </summary>
	/// <param name="dc"></param>
	void Render(ID3D11DeviceContext* dc);

	void SetAnimationActor(ID3D11Device* device, const std::shared_ptr<Actor>& animation_actor);

	void ImGuiRender();

private:

	void ActivateRenderTarget(ID3D11DeviceContext* dc);
	void RenderAnimationActor(ID3D11DeviceContext* dc) const;
	void DeactivateRenderTarget(ID3D11DeviceContext* dc) const;

	/// <summary>
	/// ���݃r���[�ōĐ����̃A�j���[�V�����ƈقȂ�ꍇ�A
	/// �V�����A�j���[�V������ǂݍ��݁A�ݒ肷��
	/// �A�j���[�V�������X�V���ꂽ�ꍇ��true��Ԃ�
	/// </summary>
	bool LoadAnimation(const char* animation_filepath);

private:

	D3D11_VIEWPORT viewport;
	RenderTarget render_target;
	DepthStencil depth_stencil;

	std::weak_ptr<Actor> w_animation_actor;
	std::unique_ptr<GLTFSkinnedMesh> animation_model;
	std::weak_ptr<Animation> w_current_actor_animation;
	std::vector<const char*> current_animation_registerd_names;

	float animation_timer;
	bool animation_loop_flag;
};
