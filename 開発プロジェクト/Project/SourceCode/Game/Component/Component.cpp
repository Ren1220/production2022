#include "Component.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/Actor/Actor.h"

/* --- 定義 ---- */

std::shared_ptr<Actor> Component::GetActor() const
{
	return w_actor.lock();
}
