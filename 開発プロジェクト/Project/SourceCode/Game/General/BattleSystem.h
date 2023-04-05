#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <functional>
#include <memory>
#include <vector>

#include "Utility/Singleton.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

class BattleSystem: public Singleton<BattleSystem>
{
public:

	void Initialize();

	/// <summary>
	/// �퓬�����̑O�Ɋe�A�N�^�̔�_���[�W�t���O�����Z�b�g����
	/// </summary>
	void ResetHealthFlags();

	/// <summary>
	/// �����Ă����퓬���������s����
	/// </summary>
	void CombatProcess();

	/// <summary>
	/// �_���[�W�C�x���g�𑗂�A��ł܂Ƃ߂ă_���[�W�������s��
	/// </summary>
	/// <param name="attacker">�U����</param>
	/// <param name="target">�U���Ώ�</param>
	/// <param name="damage">�^����_���[�W</param>
	/// <param name="invinsible_time">�ݒ肷�閳�G����</param>
	/// <param name="camera_shake_time">�J�����V�F�C�N����</param>
	/// <param name="combat_callback">�ǉ��Ŏ��s����֐� void�iActor* attacker, Actor* target, bool is_damaged�j</param>
	void SendCombatData(
		const std::shared_ptr<Actor>& attacker,
		const std::shared_ptr<Actor>& target,
		int32_t damage,
		float invinsible_time = 0.1f,
		float camera_shake_time = 0.2f,	
		const std::function<void(Actor*, Actor*, bool)>& combat_callback = nullptr);

private:

	struct CombatData
	{
		std::weak_ptr<Actor> w_attacker; // �U����
		std::weak_ptr<Actor> w_target;   // �U���Ώ�

		int32_t damage;				// �^����_���[�W��
		float   invinsible_time;	// �ݒ肷�閳�G���
		float   camera_shake_time;	// �ݒ肷��J�����V�F�C�N����

		// �ǉ��Ŏ��s����֐� ���������U�����A���������U���ΏہA��O�͍U��������������
		std::function<void(Actor*, Actor*, bool)> combat_callback;
	};

	std::vector<CombatData> combat_datas;
};
