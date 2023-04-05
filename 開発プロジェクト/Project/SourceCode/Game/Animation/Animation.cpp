#include "Animation.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <ranges>

#include "Graphics/Model/GLTFSkinnedMesh.h"

/* --- 定義 ---- */

Animation::Animation(size_t animation_index)
	: animation_index(animation_index)
{

}

void Animation::AnimateModelNode(GLTFSkinnedMesh* model, float& time, bool& has_played)
{
	has_played = model->Animate(animation_index, time);
}

void Animation::AnimateModelNodeLoop(GLTFSkinnedMesh* model, float& time, bool& has_looped)
{
	model->AnimateLoop(animation_index, time, has_looped);

	if (has_looped)
	{
		InitializeAnimationEventsPerLoop();
	}

	return ;
}

void Animation::AddEventPerFrame(
	const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
	float event_begin_time,
	float event_end_time)
{
	animation_events.emplace_back(
		std::make_unique<AnimationEventPerFrame>(event_function, event_begin_time, event_end_time));
}

void Animation::AddEventOncePerLoop(
	const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
	float evente_begin_time)
{
	animation_events.emplace_back(
		std::make_unique<AnimationEventOncePerLoop>(event_function, evente_begin_time));
}

void Animation::SetEventWhenExit(
	const std::function<void(const std::shared_ptr<Actor>&)>& exit_event_function)
{
	animation_event_when_exit = std::make_unique<AnimationCallbackEvent>(exit_event_function);
}

void Animation::SetEventWhenEnter(
	const std::function<void(const std::shared_ptr<Actor>&)>& enter_event_function)
{
	animation_event_when_enter = std::make_unique<AnimationCallbackEvent>(enter_event_function);
}

float Animation::MeasureAnimationTimeEnd(const GLTFSkinnedMesh* model) const
{
	return model->GetAnimationLength(animation_index);
}

void Animation::ExecuteActiveEvent(
	const std::shared_ptr<Actor>& actor, float current_animation_time)
{
	for (const auto& animation_event : animation_events)
	{
		if (animation_event->CanExecuteEvent(current_animation_time))
		{
			animation_event->Execute(actor);
		}
	}
}

void Animation::ExecuteEventWhenEnter(const std::shared_ptr<Actor>& actor)
{
	if (animation_event_when_enter)
	{
		animation_event_when_enter->Execute(actor);
	}
}

void Animation::ExecuteEventWhenExit(const std::shared_ptr<Actor>& actor)
{
	if (animation_event_when_exit)
	{
		animation_event_when_exit->Execute(actor);
	}
}

void Animation::InitializeAnimationEventsPerLoop()
{
	for (const auto& animation_event : animation_events)
	{
		animation_event->InitializePerLoop();
	}
}
