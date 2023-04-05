#include "StateMachine.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "StateMachineState.h"

/* --- 定義 ---- */


StateMachine::StateMachine(uint32_t state_key)
	: Behavior(), current_state(nullptr), registered_states(),
	current_state_key(state_key), current_sub_state_key(std::nullopt),
	next_state_key(state_key), next_sub_state_key(std::nullopt), can_transition(false)
{
}

StateMachine::StateMachine(uint32_t state_key, uint32_t sub_state_key)
	: Behavior(), current_state(nullptr), registered_states(),
	current_state_key(state_key), current_sub_state_key(sub_state_key),
	next_state_key(state_key), next_sub_state_key(sub_state_key), can_transition(false)
{
}

StateMachine::~StateMachine() = default;

void StateMachine::Start()
{
	// 最初は次のキーと一致するようにしておく
	next_state_key = current_state_key;
	next_sub_state_key = current_sub_state_key;

	if (current_sub_state_key.has_value())
	{
		TransitionStateAndSubState(current_state_key, current_sub_state_key.value());
	}
	else
	{
		TransitionState(current_state_key);
	}
}

void StateMachine::Update(float elapsed_time)
{
	UpdateState();

	current_state->Execute(elapsed_time);
}

void StateMachine::End()
{
}

void StateMachine::ImGuiRender()
{
}

void StateMachine::UpdateState()
{
	// 現在のステートのインデックスを取得する
	current_state_key = FindCurrentStateKey();
	current_sub_state_key = current_state->FindCurrentSubStateKey();

	// 遷移フラグの決定
	bool trantition_state{ current_state_key != next_state_key };
	bool trantition_sub_state{ current_sub_state_key != next_sub_state_key };

	// 両ステート遷移する
	if (trantition_state && trantition_sub_state)
	{
		TransitionStateAndSubState(next_state_key, next_sub_state_key.value());
	}
	// ステートのみ 
	else if (trantition_state)
	{
		TransitionState(next_state_key);
		// この処理に入る場合遷移先のステートにサブステートが存在しないので、
		// 空の状態を設定しておく
		next_sub_state_key = std::nullopt;
	}
	// サブステートのみ
	else if (trantition_sub_state)
	{
		TransitionSubState(next_sub_state_key.value());
	}

}

void StateMachine::TransitionStateAndSubState(uint32_t state_key, uint32_t sub_state_key)
{
	// ss->end() => cs->end ~~~ cs->start() => ss->start()

	// 遷移前
	if (current_state)
	{
		if (auto current_sub_state = current_state->GetCurrentSubState())
			current_sub_state->End();

		current_state->End();
	}

	// 遷移
	current_state = GetStateOfKey(state_key);
	current_state->SetCurrentSubState(sub_state_key);

	// 遷移後
	current_state->Start();
	current_state->GetCurrentSubState()->Start();
}



void StateMachine::TransitionState(uint32_t state)
{
	// 遷移前ステートの終了処理
	if (current_state) current_state->End();
	current_state = registered_states.at(state);
	// 遷移後ステートの開始処理
	current_state->Start();
}

void StateMachine::TransitionSubState(uint32_t state) 
{
	current_state->TransitionSubState(state);
}

void StateMachine::RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state, uint32_t animation_key, bool animation_loop_flag)
{
	RegisterState(key, state);
}

void StateMachine::RegisterState(uint32_t key, const std::shared_ptr<StateMachineHierarchicalState>& state)
{
	registered_states.emplace(key, state);
}

void StateMachine::RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state, uint32_t animation_key, bool animation_loop_flag)
{
	registered_states.at(state_key)->RegisterSubState(sub_state_key, sub_state);
}

void StateMachine::RegisterSubState(uint32_t state_key, uint32_t sub_state_key, const std::shared_ptr<StateMachineState>& sub_state)
{
	registered_states.at(state_key)->RegisterSubState(sub_state_key, sub_state);
}

void StateMachine::SetNextState(uint32_t state)
{
	next_state_key = state;
}

void StateMachine::SetNextSubState(uint32_t state)
{
	next_sub_state_key = state;
}

uint32_t StateMachine::FindCurrentStateKey() const
{
	for (const auto& state : registered_states)
	{
		if (current_state == state.second)
		{
			return state.first;
		}
	}

	_ASSERT_EXPR(0, L"プログラム上ここを通らないようにする");
	return 0;
}


std::shared_ptr<StateMachineState> StateMachine::GetCurrentState()
{
	return current_state;
}

const std::shared_ptr<StateMachineHierarchicalState>& StateMachine::GetStateOfKey(uint32_t key) const
{
	return registered_states.at(key);
}
