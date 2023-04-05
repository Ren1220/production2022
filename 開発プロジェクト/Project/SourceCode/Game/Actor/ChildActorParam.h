#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>
#include <vector>

#include "Arithmetic/Arithmetic.h"

#include "Actor.h"

/* --- �錾 ---- */

/// <summary>
/// �q�A�N�^�Ƃ��ĐU�����ۂɒǉ��ŕK�v�ȃp�����[�^
/// </summary>
class ChildActorParam
{
public:

	/// <summary>
	/// </summary>
	/// <param name="parent">���̃A�N�^�̋�Ԃ���Ƀ��[�J�����W��ݒ肷��Bnullptr�̎��̓��[���h��Ԃ���ɂ���B</param>
	ChildActorParam(const std::shared_ptr<Actor>& owner, const std::shared_ptr<Actor>& parent)
		: local_position(), local_rotation(), local_scale(), local_transform(),
		w_owner_actor(owner), w_parent_actor(parent), w_sibling_actors(),
		conform_position_only(false)
	{

	}

	void UpdateRelationToParent();

	/// <summary>
	/// �e�A�N�^�Ƃ̑��ݎQ�Ƃ���������
	/// </summary>
	void UnlinkParentActor();

	std::shared_ptr<Actor> GetParentActor() const { return w_parent_actor.lock(); }

	const std::vector<std::weak_ptr<Actor>>& GetWSiblingActors(size_t index) const { return w_sibling_actors; };
	//std::shared_ptr<Actor> FindSiblingActor(const char* name);


	Vector3		          local_position;		// ���[�J�����W
	Quaternion	          local_rotation;		// ���[�J����]
	Vector3		          local_scale;			// ���[�J���X�P�[��
	Matrix		          local_transform;		// ���[�J����ԍs��

	std::weak_ptr<Actor> w_owner_actor;	 // ���̃p�����[�^�����q�A�N�^
	std::weak_ptr<Actor> w_parent_actor; // owner_actor �̐e�A�N�^
	std::vector<std::weak_ptr<Actor>> w_sibling_actors;

	bool conform_position_only;
};
