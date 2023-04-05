#include "AnimationEvent.h"

/* --- 定義 ---- */

AnimationCallbackEvent::AnimationCallbackEvent(
	const std::function<void(const std::shared_ptr<Actor>&)>& event_function)
	: event_function(event_function)
{
}

void AnimationCallbackEvent::Execute(const std::shared_ptr<Actor>& actor)
{
	event_function(actor);
}

AnimationEventOncePerLoop::AnimationEventOncePerLoop(
	const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
	float evente_begin_time)
	: AnimationCallbackEvent(event_function), 
	  evente_begin_time(evente_begin_time), 
	  has_executed_once_in_this_loop(false)
{
}

void AnimationEventOncePerLoop::InitializePerLoop()
{
	has_executed_once_in_this_loop = false;
}

bool AnimationEventOncePerLoop::CanExecuteEvent(float time) const
{
	if (has_executed_once_in_this_loop) return false;

	if (evente_begin_time <= time)
	{
		return true;
	}

	return false;
}

void AnimationEventOncePerLoop::Execute(const std::shared_ptr<Actor>& actor)
{
	event_function(actor);
	// イベントをこのループにおいて一度実行した
	has_executed_once_in_this_loop = true;
}

AnimationEventPerFrame::AnimationEventPerFrame(
	const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
	float event_begin_time, float event_end_time)
	: AnimationCallbackEvent(event_function), 
	  event_execute_time_range(event_begin_time, event_end_time)
{
}

bool AnimationEventPerFrame::CanExecuteEvent(float time) const
{
	// execute_begin <= time < execute_end
	if (event_execute_time_range.first <= time &&
		time < event_execute_time_range.second)
	{
		return true;
	}

	return false;
}
