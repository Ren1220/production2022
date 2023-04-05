#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Component.h"
#include "Graphics/Resource/Camera/Camera.h"

/* --- �錾 ---- */

class ComponentCamera : public Component
{
public:

	ComponentCamera(const std::shared_ptr<Camera>& camera, bool is_main_camera);
	~ComponentCamera() override = default;

	/// <summary>
	/// �J�n����
	/// </summary>
	void Start() override;
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender() override;
	
	/// <summary>
	/// �萔�o�b�t�@�ŎQ�Ƃ���J�������Ƃ��Ă��̃J�������g�p����
	/// </summary>
	void SetToFrameCamera();

	bool IsMainCamera() const { return is_main_camera; }

private:

	std::shared_ptr<Camera> camera;

	bool is_main_camera;

};
