#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

enum class HealthEffectID
{
	Poisonous, // �ŏ��
	Burning,   // ����

	ElementCount,
};

class HealthEffect
{
public:

	HealthEffect(float effect_time)
		: effect_timer(effect_time) {}
	virtual ~HealthEffect() = default;

	virtual void Begin(const std::shared_ptr<Actor>& actor) {};
	virtual void End(const std::shared_ptr<Actor>& actor) {};

	virtual void Update(float elapsed_time, const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// ���̃G�t�F�N�g�̌��ʂ����s����
	/// </summary>
	virtual void Apply(const std::shared_ptr<Actor>& actor, int32_t& out_current_health) = 0;

	/// <summary>
	/// ���ʎ��Ԃ��I��������
	/// </summary>
	bool IsFinished() const { return effect_timer < 0.0f; }

protected:

	std::unique_ptr<EffekseerHandle> effekseer_effect_handle;

private:

	// �G�t�F�N�g�̌��ʎ���
	float effect_timer;
};

class HealthEffectPoisonus : public HealthEffect
{
public:
	HealthEffectPoisonus(float effect_time)
		: HealthEffect(effect_time) {}
	~HealthEffectPoisonus() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;
	void End(const std::shared_ptr<Actor>& actor) override;

	/// <summary>
	/// ���̃G�t�F�N�g�̌��ʂ����s����
	/// </summary>
	void Apply(const std::shared_ptr<Actor>& actor, int32_t& out_current_health) override;

};

class HealthEffectBurning : public HealthEffect
{
public:
	HealthEffectBurning(float effect_time)
		: HealthEffect(effect_time) {}
	~HealthEffectBurning() override = default;

	void Begin(const std::shared_ptr<Actor>& actor) override;
	void End(const std::shared_ptr<Actor>& actor) override;

	/// <summary>
	/// ���̃G�t�F�N�g�̌��ʂ����s����
	/// </summary>
	void Apply(const std::shared_ptr<Actor>& actor, int32_t& out_current_health) override;

};