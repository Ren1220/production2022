#include "StateMachineState.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Game/Actor/Actor.h"
#include "Game/Animation/Animator.h"

#include "StateMachine.h"

/* --- ��` ---- */

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
	// �J�ڑO�T�u�X�e�[�g�̏I������
	if (current_sub_state) current_sub_state->End();
	current_sub_state = sub_states.at(key);
	// �J�ڌ�̃T�u�X�e�[�g�̊J�n����
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
		// ���݂̃T�u�X�e�[�g�ƈ�v����z��̃L�[��Ԃ�
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
