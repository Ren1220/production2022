#include "StateMachinePlayer.h"

/* --- ’è‹` ---- */

StateMachinePlayer::StateMachinePlayer(uint32_t state_key)
	: StateMachine(state_key)
{
}

StateMachinePlayer::StateMachinePlayer(uint32_t state_key, uint32_t sub_state_key)
	: StateMachine(state_key, sub_state_key)
{
}

StateMachinePlayer::~StateMachinePlayer()
{
}
