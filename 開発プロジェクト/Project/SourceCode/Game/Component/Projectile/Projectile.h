#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../Component.h"

/* --- �錾 ---- */

class Projectile : public Component
{
public:

	Projectile(const std::shared_ptr<Actor>& owner, float alive_time);
	~Projectile() override = default;

protected:

	/// <summary>
	/// �������Ԃ��o�߂����A�����\���Ԃ𒴂����ꍇ�j��
	/// </summary>
	/// <param name="elapsed_time"></param>
	void CheckAlived(float elapsed_time);

	/// <summary>
	/// �ǂɏՓˎ��j������
	/// </summary>
	void RemoveWhenCollideWall();

	/// <summary>
	/// �ΏۂɃ_���[�W��^����
	/// </summary>
	virtual void DamageTo(
		const std::shared_ptr<Actor>& dst, 
		int32_t val);


	float alive_time; // �����\����
	float timer;	  // ��������
	
	// ���̒e�𐶂ݏo�����A�N�^
	std::weak_ptr<Actor> w_owner;
};
