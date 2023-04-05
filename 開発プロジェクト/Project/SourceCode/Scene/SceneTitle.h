#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <stdint.h>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>

#include "Scene.h"

#include "Arithmetic/Arithmetic.h"

#include "Graphics/Resource/Texture/Texture.h"
#include "Graphics/Resource/DepthStencil.h"
#include "Graphics/Resource/RenderTarget.h"
#include "Graphics/Resource/ConstantBuffer.h"
#include "Graphics/Shader/Sprite2DShader.h"

/* --- �錾 ---- */

class SceneTitle : public Scene
{
public:
	SceneTitle();
	~SceneTitle() override = default;

public:

	void Initialize() override;
	void EndProcess() override;
	void Update(float elapsed_time) override;
	void Render(ID3D11DeviceContext* dc) override;
	void ImGuiRender() override;


private:

	/// <summary>
	/// ��ʃT�C�Y��ύX�����ۂɍs������
	/// </summary>
	void ResetScreen();

	std::shared_ptr<RenderTarget> main_render_target; // ���C�������_�[�^�[�Q�b�g
	std::shared_ptr<DepthStencil> main_depth_stencil; // ���C�������_�[�^�[�Q�b�g

	Sprite2DShader sprite_shader;
	Shader deferred_shader;

};
