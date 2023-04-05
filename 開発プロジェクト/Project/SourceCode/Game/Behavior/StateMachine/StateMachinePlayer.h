#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "StateMachine.h"

/* --- 宣言 ---- */

class StateMachinePlayer : public StateMachine
{
public:

	StateMachinePlayer(uint32_t state_key);
	StateMachinePlayer(uint32_t state_key, uint32_t sub_state_key);

	~StateMachinePlayer () override;

private:


};
