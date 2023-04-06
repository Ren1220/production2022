#include "ChildActorParam.h"

#include <algorithm>

/* --- ��` ---- */

void ChildActorParam::UpdateRelationToParent()
{
	if (w_parent_actor.expired())
	{
		w_owner_actor.lock()->SetChildActorFlag(false);
		return;
	}

	// �Q�Ƃ��؂ꂽ�Z��A�N�^��z�񂩂珜�O����
	auto itr_erase_expired_sibling_actors{
		std::ranges::remove_if(w_sibling_actors, [](const auto& wptr) {return wptr.expired(); }) };
	w_sibling_actors.erase(itr_erase_expired_sibling_actors.begin(), w_sibling_actors.end());
}

void ChildActorParam::UnlinkParentActor()
{
	GetParentActor()->RemoveChildActor(w_owner_actor.lock());
}
