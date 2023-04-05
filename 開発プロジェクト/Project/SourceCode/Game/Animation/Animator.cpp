#include "Animator.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <ranges>

#include "Game/Actor/Actor.h"

/* --- 定義 ---- */

Animator::Animator()
	: animation_timer1(), animation_timer2(), is_loop(), has_looped(), has_played_animation(), next_animation(), current_animation(), interpolation(), blend_ratio()
{
}

void Animator::Start()
{

}

void Animator::End()
{
}

void Animator::Update(float elapsed_time)
{
	animation_timer1 += elapsed_time;
	animation_timer2 += elapsed_time;

	bool b[2]{ current_animation.has_value(), next_animation.has_value() };

	// 現在再生中のアニメーション、次に再生するアニメーションの両方が設定されている場合は、アニメーションブレンドを行う
	if (b[0] && b[1])
	{
		size_t cur{ current_animation.value()->second->GetAnimationIndex() };
		size_t next{ next_animation.value()->second->GetAnimationIndex() };

		// ブレンド率（次のアニメーションの比率）
		blend_ratio = std::lerp(blend_ratio, 1.0f, interpolation * elapsed_time);

		// アニメーションブレンド
		GetActor()->GetModel<GLTFSkinnedMesh>()->BlendAnimation(cur, next, animation_timer1, animation_timer2, blend_ratio);

		// ブレンド率が一定値以上でブレンドを終了し次のアニメーションに完全に遷移する。
		if (blend_ratio >= 0.99f)
		{
			// 遷移前のアニメーション終了イベント
			if (auto exit_animaiton = current_animation.value()->second)
			{
				exit_animaiton->ExecuteEventWhenExit(GetActor());
				exit_animaiton->InitializeAnimationEventsPerLoop();
			}

			animation_timer1 = animation_timer2;
			current_animation.reset();
		}
	}
	// 特にブレンドしていない
	else if (b[0])
	{
		auto actor_model{ GetActor()->GetModel<GLTFSkinnedMesh>() };

		if (is_loop)
		{
			current_animation.value()->second->AnimateModelNodeLoop(actor_model, animation_timer1, has_looped);
		}
		else
		{
			current_animation.value()->second->AnimateModelNode(actor_model, animation_timer1, has_played_animation);
		}
	}
	// 現在再生中のアニメーションが存在しない
	else if (b[1])
	{
		current_animation = next_animation;
		next_animation.reset();

		auto actor_model{ GetActor()->GetModel<GLTFSkinnedMesh>() };

		if (is_loop)
		{
			current_animation.value()->second->AnimateModelNodeLoop(actor_model, animation_timer1, has_looped);
		}
		else
		{
			current_animation.value()->second->AnimateModelNode(actor_model, animation_timer1, has_played_animation);
		}
	}
}

void Animator::AnimationEvent()
{
	// 再生中のアニメーションイベントが存在するか
	if (current_animation.has_value())
	{
		auto animation{ current_animation.value()->second };

		// この時間に実行可能なアニメーションイベントを実行する
		animation->ExecuteActiveEvent(GetActor(), animation_timer1);
	}

	// 再生中のアニメーションイベントが存在するか
	if (next_animation.has_value())
	{
		auto animation{ next_animation.value()->second };

		// この時間に実行可能なアニメーションイベントを実行する
		animation->ExecuteActiveEvent(GetActor(), animation_timer2);
	}
}

void Animator::ImGuiRender()
{
	if (auto animation = GetCurrentPlayAnimation())
	{
		float animation_end{ animation->MeasureAnimationTimeEnd(GetActor()->GetModel<GLTFSkinnedMesh>()) };
		ImGui::SliderFloat("AnimationTimer", &animation_timer1, 0.0f, animation_end);
		ImGui::RadioButton("IsLoop", is_loop);
		ImGui::Text("ModelAnimationIndex %d", static_cast<int>(animation->GetAnimationIndex()));
	}
	else
	{
		ImGui::Text("NotAnimationPlaying");
	}
}


const std::shared_ptr<Animation>& Animator::RegisterAnimation(uint32_t key, size_t animation_index)
{
	auto ptr{ std::make_shared<Animation>(animation_index) };
	auto result{ registered_animations.try_emplace(key, ptr) };
	_ASSERT_EXPR(result.second, L"既に同じキーにアニメーションが登録されています。");
	return result.first->second;
}

void Animator::PlayAnimation(uint32_t key, bool is_loop, float interpolation)
{
	auto actor{ GetActor() };
	
	// ブレンドせずに遷移
	if (interpolation >= FLT_MAX - FLT_EPSILON)
	{
		// 前回再生していたアニメーションの終了イベント
		if (auto exit_animaiton = GetCurrentPlayAnimation())
		{
			exit_animaiton->ExecuteEventWhenExit(actor);
			exit_animaiton->InitializeAnimationEventsPerLoop();
		}

		// パラメータ設定
		SetCurrentAnimation(key);
		next_animation.reset();
		animation_timer1 = 0.0f;
		this->is_loop = is_loop;
		has_played_animation = false;

		// 新しく再生するアニメーションの開始イベント
		if (auto enter_animation = GetCurrentPlayAnimation())
		{
			enter_animation->ExecuteEventWhenEnter(actor);
			enter_animation->InitializeAnimationEventsPerLoop();
		}
	}
	// ブレンドしつつ遷移
	else
	{
		// ブレンド中に再び遷移した場合の処理
		if (next_animation.has_value()) 
		{
			auto exit_animaiton = next_animation.value()->second;

			exit_animaiton->ExecuteEventWhenExit(actor);
			exit_animaiton->InitializeAnimationEventsPerLoop();
			
			current_animation = next_animation;
		}

		// 前回再生していたアニメーションの終了イベント
		if (auto exit_animaiton = GetCurrentPlayAnimation())
		{
			exit_animaiton->ExecuteEventWhenExit(actor);
		}

		next_animation = registered_animations.find(key);
		this->interpolation = interpolation;
		blend_ratio = 0.0f;
		animation_timer2 = 0.0f;
		has_played_animation = false;
		this->is_loop = is_loop;

		// 新しく再生するアニメーションの開始イベント
		if (auto enter_animation = next_animation.value()->second)
		{
			enter_animation->ExecuteEventWhenEnter(actor);
			enter_animation->InitializeAnimationEventsPerLoop();
		}
	}
}

void Animator::StopAnimation()
{
	auto actor{ GetActor() };

	// 再生していたアニメーションの終了イベント
	if (auto exit_animaiton = GetCurrentPlayAnimation())
	{
		exit_animaiton->ExecuteEventWhenExit(actor);
		exit_animaiton->InitializeAnimationEventsPerLoop();
	}

	current_animation.reset();
	animation_timer1 = FLT_MAX;
	is_loop = false;
	has_played_animation = true;
}

void Animator::SetActor(const std::shared_ptr<Actor>& actor)
{
	w_actor = actor;
}

std::shared_ptr<Actor> Animator::GetActor() const
{
	return w_actor.lock();
}

void Animator::SetCurrentAnimation(uint32_t key)
{
	current_animation = registered_animations.find(key);
}

std::shared_ptr<Animation> Animator::GetCurrentPlayAnimation() const
{
	if (current_animation.has_value())
	{
		return current_animation.value()->second;
	}

	return nullptr;
}

const std::shared_ptr<Animation>& Animator::GetAnimation(uint32_t key) const
{
	return registered_animations.at(key);
}

