#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "StateMachine.h"

/* --- �錾 ---- */

class StateMachinePlayer : public StateMachine
{
public:

	StateMachinePlayer(uint32_t state_key);
	StateMachinePlayer(uint32_t state_key, uint32_t sub_state_key);

	~StateMachinePlayer () override;

private:


};
