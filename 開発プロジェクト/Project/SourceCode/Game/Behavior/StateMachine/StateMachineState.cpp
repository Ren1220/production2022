#include "StateMachineState.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/Animation/Animator.h"

#include "StateMachine.h"

/* --- 定義 ---- */

StateMachineState::StateMachineState(const std::shared_ptr<Actor>& actor)
	:w_actor(actor)
{

}

StateMachineState::~StateMachineState() = default;

std::shared_ptr<StateMachine> StateMachineState::GetStateMachine() const
{
	if (auto actor = w_actor.lock())
	{
		return actor->GetBehavior<StateMachine>();
	}

	return nullptr;
}

std::shared_ptr<Actor> StateMachineState::GetActor() const
{
	return w_actor.lock();
}

void StateMachineState::StopAnimation() const
{
	if (auto actor = GetActor())
	{
		actor->GetAnimator()->StopAnimation();
	}
}

StateMachineHierarchicalState::StateMachineHierarchicalState(const std::shared_ptr<Actor>& actor)
	: StateMachineState(actor)
{

}

StateMachineHierarchicalState::~StateMachineHierarchicalState() = default;

void StateMachineHierarchicalState::TransitionSubState(uint32_t key)
{
	// 遷移前サブステートの終了処理
	if (current_sub_state) current_sub_state->End();
	current_sub_state = sub_states.at(key);
	// 遷移後のサブステートの開始処理
	current_sub_state->Start();
}

void StateMachineHierarchicalState::RegisterSubState(uint32_t key, std::shared_ptr<StateMachineState> state)
{
	sub_states.emplace(key, state);
}

std::shared_ptr<StateMachineState> StateMachineHierarchicalState::GetCurrentSubState()
{
	return current_sub_state;
}

void StateMachineHierarchicalState::SetCurrentSubState(uint32_t key)
{
	current_sub_state = GetSubStateOfKey(key);
}

const std::shared_ptr<StateMachineState>& StateMachineHierarchicalState::GetSubStateOfKey(uint32_t key) const
{
	return sub_states.at(key);
}

std::optional<uint32_t> StateMachineHierarchicalState::FindCurrentSubStateKey()
{
	for (const auto& sub_state : sub_states)
	{
		// 現在のサブステートと一致する配列のキーを返す
		if (current_sub_state == sub_state.second)
		{
			return sub_state.first;
		}
	}

	return std::nullopt;
}

void StateMachineHierarchicalState::ExecuteSubState(float elapsed_time) const
{
	current_sub_state->Execute(elapsed_time);
}
