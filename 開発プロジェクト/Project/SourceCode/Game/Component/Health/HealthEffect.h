#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

enum class HealthEffectID
{
	Poisonous, // 毒状態
	Burning,   // 炎上

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
	/// このエフェクトの効果を実行する
	/// </summary>
	virtual void Apply(const std::shared_ptr<Actor>& actor, int32_t& out_current_health) = 0;

	/// <summary>
	/// 効果時間が終了したか
	/// </summary>
	bool IsFinished() const { return effect_timer < 0.0f; }

protected:

	std::unique_ptr<EffekseerHandle> effekseer_effect_handle;

private:

	// エフェクトの効果時間
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
	/// このエフェクトの効果を実行する
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
	/// このエフェクトの効果を実行する
	/// </summary>
	void Apply(const std::shared_ptr<Actor>& actor, int32_t& out_current_health) override;

};