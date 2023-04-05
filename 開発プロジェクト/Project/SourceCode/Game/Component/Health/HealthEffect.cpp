#include "HealthEffect.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Effekseer/EffekseerHandle.h"

#include "../../Actor/Actor.h"

#include "Health.h"

/* --- 定義 ---- */

void HealthEffect::Update(float elapsed_time, const std::shared_ptr<Actor>& actor)
{
	effect_timer -= elapsed_time;

	if (effekseer_effect_handle)
	{
		effekseer_effect_handle->SetPosition(actor->GetWorldPosition() + Vector3(0, 2, 0));
	}
}

void HealthEffectPoisonus::Begin(const std::shared_ptr<Actor>& actor)
{
	effekseer_effect_handle = std::make_unique<EffekseerHandle>();
	effekseer_effect_handle->Initialize("Poison/poison.efk", actor->GetWorldPosition() + Vector3(0, 2, 0));
	effekseer_effect_handle->SetScale({ 0.3f, 0.3f, 0.3f });
}

void HealthEffectPoisonus::End(const std::shared_ptr<Actor>& actor)
{
	effekseer_effect_handle->Destroy();
}

void HealthEffectPoisonus::Apply(
	const std::shared_ptr<Actor>& actor, 
	int32_t& out_current_health)
{
	out_current_health -= 1;
}

void HealthEffectBurning::Begin(const std::shared_ptr<Actor>& actor)
{
	effekseer_effect_handle = std::make_unique<EffekseerHandle>();
	effekseer_effect_handle->Initialize("Fire/fire.efk", actor->GetWorldPosition());
}

void HealthEffectBurning::End(const std::shared_ptr<Actor>& actor)
{
	effekseer_effect_handle->Destroy();
}

void HealthEffectBurning::Apply(
	const std::shared_ptr<Actor>& actor,
	int32_t& out_current_health)
{
	out_current_health -= 1;
}

