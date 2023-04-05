#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Actor.h"

/* --- �錾 ---- */

class ActorWeapon : public Actor
{
public:

	ActorWeapon()
		: Actor()
	{

	}

	/// <summary>
	/// �������B�R���|�[�l���g�A�����p�����[�^��ݒ肷��B
	/// </summary>
	/// <param name="device"></param>
	/// <param name="name"></param>
	/// <param name="attack_power">����U����</param>
	/// <param name="durability">����ϋv�l</param>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		int32_t attack_power,
		int32_t durability,
		const Vector3& position,
		const Quaternion& rotation = Quaternion::Identity(),
		const Vector3& scale = { 1, 1, 1 });
};
