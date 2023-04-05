#include "ComponentCamera.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"

#include "../../Actor/Actor.h"
#include "../../Actor/ChildActorParam.h"

/* --- 定義 ---- */

ComponentCamera::ComponentCamera(const std::shared_ptr<Camera>& camera, bool is_main_camera)
	: Component(), camera(camera)
{
	SetToFrameCamera();
}

void ComponentCamera::Start()
{

}

void ComponentCamera::Update(float elapsed_time)
{
	auto actor{ GetActor() };

	camera->SetEye(actor->GetWorldPosition());

	if (auto parent = actor->GetChildActorParam()->GetParentActor())
	{
		camera->SetFocus(parent->GetWorldPosition());
	}

	camera->Update(elapsed_time);

}

void ComponentCamera::ImGuiRender()
{
	camera->ImGuiRender();

	if(ImGui::Button("SetToFrameCamera"))
	{
		SetToFrameCamera();
	}
}

void ComponentCamera::SetToFrameCamera()
{
	Graphics::GetInstance().GetFrameBuffer()->PrepareCamera(camera);
}
