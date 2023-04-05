#include "Health.h"


/* --- 追加のヘッダファイルのインクルード ---- */


#include "Game/Actor/Actor.h"
#include "Game/Animation/Animator.h"
#include "Game/General/GameSystem.h"
#include "Game/General/Tutorial.h"
#include "Game/HUD/HUDManager.h"
#include "Graphics/General/Graphics.h"
#include "Graphics/Resource/FrameBuffer.h"
#include "Graphics/Resource/Camera/Shake/CameraShakeDamaged.h"

/* --- 定義 ---- */

Health::Health(int32_t current_health, int32_t max_health)
	: health(current_health), max_health(max_health), damage(0), invincible_timer(-1.0f), apply_effect_timer(0.0f),
	is_dead(false), is_damaged(false)
{
}

Health::~Health()
{
}

void Health::Start()
{
}

void Health::End()
{
	auto actor{ GetActor() };

	// 状態エフェクトを終了する
	for (auto& e : health_effects)
	{
		if (e)
		{
			e->End(actor);
			e.reset();
		}
	}

	w_attackers.clear();
}

void Health::Update(float elapsed_time)
{
	auto actor{ GetActor() };

	// 無敵時間を経過させる(-1.0fを下回らないようにする)
	invincible_timer = (std::max)(invincible_timer - elapsed_time, -1.0f);

	// 各状態の更新
	for (auto& e : health_effects)
	{
		if (!e) continue;

		e->Update(elapsed_time, actor);

		if (e->IsFinished())
		{
			e->End(actor);
			e.reset();
		}
	}

	w_attackers.clear();


	// 一秒毎にエフェクト効果を実行する
	if (apply_effect_timer > 1.0f)
	{
		ApplyEffect();
		apply_effect_timer = 0.0f;
	}

	apply_effect_timer += elapsed_time;

}

void Health::ImGuiRender()
{
	ImGui::InputInt("CurrentHealth", &health);
	ImGui::InputInt("MaxHealth", &max_health);
	ImGui::InputFloat("InvincibleTimer", &invincible_timer, 0, 0, "%.1f");

	if (ImGui::CollapsingHeader("HealthEffect"))
	{
		if (ImGui::Button("Poisonous"))
		{
			SetEffect(HealthEffectID::Poisonous, 5.0f);
		}
		if (ImGui::Button("Burning"))
		{
			SetEffect(HealthEffectID::Burning, 5.0f);
		}
	}
}

bool Health::Damaged(const std::shared_ptr<Actor>& attacker, int32_t damage, float invincible_time, float camera_shake_time)
{
	// 攻撃してきた相手を登録する
	w_attackers.emplace_back(attacker);

	// 無敵時間でない
	if (invincible_timer < 0)
	{
		// health -= damage;
		this->damage += damage;

		// ダメージの反映
		health -= damage;

		is_damaged = true; 

		// 無敵時間を設定する
		invincible_timer = invincible_time;

		// 体力がゼロ
		if (health < 1)
		{
			// チュートリアル時
			if (Tutorial::GetInstance().IsTutorial())
			{
				if (Tutorial::GetInstance().GetFlag(Tutorial::ShowCounterText) != Tutorial::AlreadyDisplayed)
				{
					// チュートリアルがある程度進まないと死なない
					health = 1;
				}
				else
				{
					is_dead = true;

					if (GetActor()->CompareGroupName("Player"))
					{
						GameSys.GameOver();
						invincible_timer = 5.0f;
					}

				}
			}
			else
			{
				is_dead = true;

				if (GetActor()->CompareGroupName("Player"))
				{
					GameSys.GameOver();
					invincible_timer = 5.0f;
				}
			}
		}


		// カメラシェイクを行う
		const auto& frame_camera{ Graphics::GetInstance().GetFrameBuffer()->GetFrameCamera() };
		frame_camera->SetCameraShake(std::make_unique<CameraShakeDamaged>(camera_shake_time, 1.0f, 1.0f));

		// ダメージの表示
		GameSys.GetHUDManager()->GetDamage().AddDatas(GetActor(), 2.0f, damage);

		return true;

	}

	return false;
}

void Health::Recover(int32_t value)
{
	// 最大体力を超えないようにする
	health = std::clamp(health + value, -1, max_health);
}

void Health::ResetDamagedFlag()
{
	is_damaged = false; 
	damage = 0;
}

void Health::SetEffect(HealthEffectID id, float time)
{
	auto& effect{ health_effects[static_cast<size_t>(id)] };

	auto actor{ GetActor() };

	if (effect)
	{
		effect->End(actor);
	}

	switch (id)
	{
	case HealthEffectID::Poisonous:
		effect = std::make_unique<HealthEffectPoisonus>(time);
		break;
	case HealthEffectID::Burning:
		effect = std::make_unique<HealthEffectBurning>(time);
		break;
	default:
		_ASSERT_EXPR(0, L"不明な種類の効果");
		break;
	}

	effect->Begin(actor);
}

void Health::ApplyEffect()
{
	auto actor{ GetActor() };

	for (const auto& e : health_effects)
	{
		if (!e) continue;

		e->Apply(actor, health);
	}
}
