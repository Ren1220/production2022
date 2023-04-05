#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <array>

#include "../Component.h"

#include "HealthEffect.h"


/* --- �錾 ---- */


class Health : public Component
{
public:

	Health(int32_t current_health, int32_t max_health);
	~Health() override;

	void Start() override;
	void End() override;
	void Update(float elapsed_time) override;
	void ImGuiRender() override;

public:

	/// <summary>
	/// �_���[�W��^����
	/// </summary>
	/// <param name="attacker">�U�����Ă�������</param>
	/// <param name="damage">�^����_���[�W��</param>
	/// <param name="invincible_time">�ݒ肷�閳�G����</param>
	/// <param name="camera_shake_time">�J�����V�F�C�N���s������</param>
	/// <returns>�_���[�W��^����ꂽ��</returns>
	bool Damaged(
		const std::shared_ptr<Actor>& attacker,
		int32_t damage, 
		float invincible_time, 
		float camera_shake_time);

	/// <summary>
	/// �񕜂�����ő�̗͂𒴂��ĉ񕜂͂��Ȃ�
	/// </summary>
	/// <param name="value">�񕜗�</param>
	void Recover(int32_t value);

	/// <summary>
	/// ���G���Ԃ�ݒ肷��
	/// </summary>
	void SetInvincibleTimer(float t) { invincible_timer = t; }

	int32_t GetHealth() const { return health; }
	int32_t GetMaxHealth() const { return max_health; }
	float GetInvincibleTimer() const { return invincible_timer; }

	/// <summary>
	/// ���̃t���[���ɍU�������Ă�������i�U���̐����̉ۂ͖��Ȃ��j
	/// </summary>
	const std::vector<std::weak_ptr<Actor>>& GetAttackers() const { return w_attackers; }

	bool IsInvincible() const { return invincible_timer > 0.0f; }

	/// <summary>
	/// ��Ԉُ�ɂ��_���[�W�͊܂܂�Ȃ�
	/// </summary>
	bool IsDamaged() const { return is_damaged; }

	bool IsDead() const { return is_dead; }

	/// <summary>
	/// �_���[�W�t���O�����Z�b�g����
	/// ���̃t���[���Ɏ󂯂��_���[�W�̑��ʂ�
	/// </summary>
	void ResetDamagedFlag();

	/// <summary>
	/// ��ԂƂ��̌��ʎ��Ԃ�ݒ肷��
	/// </summary>
	void SetEffect(HealthEffectID id, float time);

private:

	/// <summary>
	/// �e��Ԃɉ��������ʂ�K�p����
	/// </summary>
	void ApplyEffect();

	int32_t damage; // ���̃t���[���Ɏ󂯂��_���[�W�̍��v�l
	int32_t health;
	int32_t max_health;

	// 0�𒴂���l�̏ꍇ���G��ԂƂ��Ĉ�����
	float invincible_timer;

	bool is_dead;
	bool is_damaged; // ��Ԉُ�ɂ��_���[�W�͊܂܂�Ȃ�

	// �G�t�F�N�g�̌��ʂ��w��b���Ɏ��s����ׂ̃^�C�}�[
	float apply_effect_timer;

	// �󂯂���Ԃɉ��������ݍĐ����̃G�t�F�N�g
	std::array<std::unique_ptr<HealthEffect>, static_cast<size_t>(HealthEffectID::ElementCount)> health_effects;

	// ���̃t���[���ɍU�������Ă�������i�U���̐����̉ۂ͖��Ȃ��j
	std::vector<std::weak_ptr<Actor>> w_attackers;
};

