#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Projectile.h"

/* --- �錾 ---- */

class ProjectileSpiderPoison : public Projectile
{
public:

	ProjectileSpiderPoison(
		const std::shared_ptr<Actor>& owner, 
		int32_t power,
		const Vector3& direction,
		float speed = 1.0f,
		float max_speed = 2.0f,
		float alive_time = 100.0f);
	~ProjectileSpiderPoison() override = default;

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
	/// �������Ɏ��s����֐�
	/// </summary>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;

private:

	/// <summary>
	/// �ΏۂɃ_���[�W��^����
	/// </summary>
	void DamageTo(
		const std::shared_ptr<Actor>& dst,
		int32_t val) override;


	int32_t power;
	Vector3 direction;
	float speed;
	float max_speed;

	
};
