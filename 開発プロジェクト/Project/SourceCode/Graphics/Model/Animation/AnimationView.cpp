#include "AnimationView.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "../../Resource/RenderTarget.h"

/* --- 定義 ---- */

void AnimationView::Initialize(ID3D11Device* device, float view_width, float view_height)
{
	//const UINT width{ static_cast<UINT>(view_width) };
	//const UINT height{ static_cast<UINT>(view_height) };
	//render_target.CreateView(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	//depth_stencil.CreateView(device, width, height);
	//
	//constexpr Vector3 eye{ 0, 1, 3 };
	//constexpr Vector3 focus{ 0, 1, 0 };
	//constexpr Vector3 up{ 0, 1, 0 };
	//scene_view.Initialize(device, eye, focus, up);

	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.Width = view_width;
	//viewport.Height = view_height;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;

	//w_animation_actor.reset();
	//animation_model = nullptr;
	//w_current_actor_animation.reset();

	//animation_timer = 0.0f;
	//animation_loop_flag = false;
}

void AnimationView::SetAnimationActor(ID3D11Device* device, const std::shared_ptr<Actor>& set_animation_actor)
{
	//if (auto animation_actor = w_animation_actor.lock())
	//{
	//	if (animation_actor == set_animation_actor)
	//	{
	//		return;
	//	}
	//}

	//if (auto animator = set_animation_actor->GetAnimator())
	//{
	//	w_animation_actor = set_animation_actor;
	//
	//	const char* actor_model_filepath{ set_animation_actor->GetModel()->GetModelData()->filepath.c_str() };
	//	animation_model = std::make_unique<GLTFSkinnedMesh>(device, actor_model_filepath);
	//	animation_model->CreateShader(device, "Shader/gltf_model_vs.cso", "Shader/gltf_model_ps.cso");
	//	animation_model->SetWorldTransform(Matrix::CreateScaling(set_animation_actor->GetWorldScale()));

	//	w_current_actor_animation= animator->GetCurrentPlayAnimation();
	//	current_animation_registerd_names = animator->MakeRegisteredAnimationNames();
	//}
	//else
	//{
	//	animation_model.reset();
	//	w_animation_actor.reset();
	//	w_current_actor_animation.reset();
	//}
}

void AnimationView::ImGuiRender()
{
	//if (ImGui::Begin("AnimationView", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
	//{
	//	ImGui::Image(render_target.GetShaderResourceView().Get(), { 320, 640 });

	//	if (ImGui::Button("Replay"))
	//	{
	//		animation_timer = 0.0f;
	//	}
	//	ImGui::SameLine();
	//	ImGui::Checkbox("Loop", &animation_loop_flag);

	//	if (auto animation_actor = w_animation_actor.lock())
	//	{
	//		auto animation { w_current_actor_animation.lock() };
	//		float animation_end{ animation->MeasureAnimationTimeEnd() };
	//		ImGui::SliderFloat("AnimationTimer", &animation_timer, 0.0f, animation_end);

	//		static int current_item{ 0 };
	//		const char* animation_filepath{ current_animation_registerd_names.at(current_item) };
	//		const int item_count{ static_cast<int>(current_animation_registerd_names.size()) };
	//		ImGui::Combo("Animations", &current_item, current_animation_registerd_names.data(), item_count);
	//		if (LoadAnimation(animation_filepath)) {}

	//	}

	//	scene_view.ImGuiRenderTreeNode();

	//	ImGui::End();
	//}
}

void AnimationView::ActivateRenderTarget(ID3D11DeviceContext* dc)
{
	//auto& render_target_interface{ RenderTargetManager::GetInstance()};

	//render_target_interface.RegisterRenderTargets(viewport, { &render_target }, &depth_stencil);
	//render_target_interface.ClearRenderTargetViews(dc);
	//render_target_interface.ClearDepthStencilView(dc);
	//render_target_interface.ActivateRenderTargets(dc);

	//scene_view.UpdateConstantBufferSubresource(viewport.Width, viewport.Height);
	//scene_view.SendConstantBufferToGPU(dc);
}

void AnimationView::DeactivateRenderTarget(ID3D11DeviceContext* dc) const 
{
	// ビューを元に戻す
	//RenderTargetManager::GetInstance().ReRegisterCachedRenderTarget();
}

bool AnimationView::LoadAnimation(const char* animation_filepath)
{
	//auto actor{ w_animation_actor.lock() };
	//auto animator{ actor->GetAnimator() };

	//if (auto current_actor_animation = w_current_actor_animation.lock())
	//{
	//	auto current_animation_name{ current_actor_animation->GetFilepath() };

	//	if (current_animation_name != animation_filepath)
	//	{

	//		w_current_actor_animation = animator->FindAnimationFromRegisters(animation_filepath);

	//		return true;
	//	}
	//}
	//else
	//{
	//	w_current_actor_animation = animator->FindAnimationFromRegisters(animation_filepath);
	//	return true;
	//}

	return false;
}

void AnimationView::UpdateCurrentAnimationData(float elapsed_time)
{
	//animation_timer += elapsed_time;

	//if (auto current_actor_animation = w_current_actor_animation.lock())
	//{
	//	bool dummy;

	//	if (animation_loop_flag)
	//	{
	//		current_actor_animation->AnimateModelNodeLoop(animation_model.get(), animation_timer, dummy);
	//	}
	//	else
	//	{
	//		current_actor_animation->AnimateModelNode(animation_model.get(), animation_timer, dummy);
	//	}
	//}
}

void AnimationView::Render(ID3D11DeviceContext* dc)
{
	ActivateRenderTarget(dc);
	RenderAnimationActor(dc);
	DeactivateRenderTarget(dc);
}

void AnimationView::RenderAnimationActor(ID3D11DeviceContext* dc) const
{
	//if (animation_model)
	//{
	//	animation_model->Render(dc);
	//}
}
