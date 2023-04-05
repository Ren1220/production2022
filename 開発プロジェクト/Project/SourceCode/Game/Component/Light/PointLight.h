#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Component.h"
#include "Graphics/PostProcess/DeferredLighting.h"

/* --- �錾 ---- */

class PointLight : public Component
{
public:

	PointLight(const Vector3& light_position, float attenuation, const Vector3& light_color, float luminance, bool is_shadow_cast);
	~PointLight() override = default;

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

	// �^�C�g���ł͐^���Âɂ��邽�߁A�ꎟ�I�ɏ����l���X�Ɋm�ۂ���
	float attenuation_delta;
	float luminance_delta;

	bool is_shadow_cast; // ���̌����ŃV���h�E���s����

};
