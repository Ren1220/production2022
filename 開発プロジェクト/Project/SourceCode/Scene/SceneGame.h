#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <cstdint>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "Scene.h"

#include "Arithmetic/Arithmetic.h"
#include "Audio/AudioHandle.h"

#include "Graphics/Resource/DepthStencil.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Resource/Camera/PerspectiveCamera.h"

#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Model/GLTFSkinnedMesh.h"

/* --- �錾 ---- */

class SceneGame : public Scene
{
public:
	SceneGame();
	~SceneGame() override = default;

public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;
	void ImGuiRender() override;

	void GameRetry(ID3D11Device* device);

private:

	/// <summary>
	/// ��ʉ𑜓x��ύX�����ۂɍs�������B
	/// �t���[���o�b�t�@�̃T�C�Y��ύX���Đ�������B
	/// </summary>
	void ResetScreen();

	bool is_game_retry;

	std::shared_ptr<RenderTarget> main_render_target; // ���C�������_�[�^�[�Q�b�g
	std::shared_ptr<DepthStencil> main_depth_stencil; // ���C�������_�[�^�[�Q�b�g


	AudioHandle bgm;

#ifdef _DEBUG
	void ControlCamera(float elapsed_time);

	void ImGuiScene();
	void ImGUiCameraControl();

	bool can_control_camera;
	std::shared_ptr<Camera> camera;
#endif // _DEBUG
};
