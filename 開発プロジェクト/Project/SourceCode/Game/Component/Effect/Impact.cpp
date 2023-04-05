#include "Impact.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"
#include "Game/General/GameSystem.h"
#include "Game/Actor/ActorManager.h"

/* --- 定義 ---- */

Impact::Impact(float rising)
	:rising(rising), displacement(0.0f), state(0)
{
}

void Impact::Start()
{
}

void Impact::Update(float elapsed_time)
{
	auto actor{ GetActor() };

	switch (state)
	{
	case 0:
		displacement = std::lerp(displacement, rising, 10.0f * elapsed_time);
		
		if (displacement <= rising - 0.01f)
		{
			state++;
		}

		break;
	case 1:

		displacement = std::lerp(displacement, -2.0f, elapsed_time * 0.1f);

		if (displacement < -2.0f + 0.01f)
		{
			GameSys.GetActorManager()->Remove(actor);
		}

		break;
	}

	Vector3 pos{ actor->GetWorldPosition() };
	pos.y = displacement;
	actor->SetWorldPosition(pos);

}

void Impact::ImGuiRender()
{
}

void Impact::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}
