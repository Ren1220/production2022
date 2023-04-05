#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include "../BehaviorTree.h"

/* --- �錾 ---- */

class BehaviorEnemy: public BehaviorTree
{
public:

	BehaviorEnemy();
	~BehaviorEnemy() override;

	virtual void Start() override;
	virtual void Update(float elapsed_time) override;

	virtual void ImGuiRender() override;

public:

	/// <summary>
	/// �O�̃t���[���Ń_���[�W���󂯂����ƂŒ��f���ꂽ�Ƃ��Ƀ_���[�W�X�e�[�g�ɑJ�ڂ���ۂɎg���B
	/// �r�w�C�r�A���Ŕ�_���ɂ�蒆�f->Health�X�V->Inference�ł̃X�e�[�g������Ƃ������ɏ�������邽�߁B
	/// </summary>
	void SetDamaged(bool b = true) { is_damaged = b; }

	/// <summary>
	/// �O�̃t���[���Ń_���[�W���󂯂����ƂŒ��f���ꂽ�Ƃ��Ƀ_���[�W�X�e�[�g�ɑJ�ڂ���ۂɎg���B
	/// �r�w�C�r�A���Ŕ�_���ɂ�蒆�f->Health�X�V->Inference�ł̃X�e�[�g������Ƃ������ɏ�������邽�߁B
	/// </summary>
	bool IsDamaged() const { return is_damaged; }

	/// <summary>
	/// �v���C���[�ւ̃x�N�g��
	/// </summary>
	const Vector3& GetPlayerVector() const { return player_vector; }

protected:

	// �O�̃t���[���Ń_���[�W���󂯂����ƂŒ��f���ꂽ�Ƃ��Ƀ_���[�W�X�e�[�g�ɑJ�ڂ���ۂɎg���B
	// �r�w�C�r�A���Ŕ�_���ɂ�蒆�f->Health�X�V->Inference�ł̃X�e�[�g������Ƃ������ɏ�������邽�߁B
	bool is_damaged; 

	// �v���C���[�Ƃ̃x�N�g���͂悭�g���̂Ōv�Z���Ă���
	Vector3 player_vector;
};

