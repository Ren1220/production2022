#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Component.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- �錾 ---- */

class DirectionalLight : public Component
{
public:

	DirectionalLight(const Vector3& light_direction, float luminance, const Vector3& light_color = { 1, 1, 1 });
	virtual ~DirectionalLight() override = default;

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

private:

	std::shared_ptr<DeferredLighting::Light> light_data;
};
