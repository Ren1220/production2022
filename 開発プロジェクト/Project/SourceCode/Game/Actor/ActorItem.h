#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Actor.h"

/* --- �錾 ---- */

class ActorItem : public Actor
{
public:

	ActorItem()
		: Actor()
	{

	}

	/// <summary>
	/// �������B�R���|�[�l���g�A�����p�����[�^��ݒ肷��B
	/// </summary>
	/// <param name="name"></param>
	/// <param name="group_name"></param>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	/// <param name="model_filepath"></param>
	/// <param name="is_static_mesh"></param>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });
};

