#include "Component.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Game/Actor/Actor.h"

/* --- ��` ---- */

std::shared_ptr<Actor> Component::GetActor() const
{
	return w_actor.lock();
}
