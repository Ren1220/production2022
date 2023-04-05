#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "Arithmetic/Arithmetic.h"

#include "../../Actor/Actor.h"

/* --- �錾 ---- */

/// <summary>
/// �s���������N���X
/// </summary>
class BehaviorAction
{
public:

	/// <summary>
	/// ���s���
	/// </summary>
	enum class State
	{
		Run,		// ���s���B���̃t���[���ɂ����ł����̃m�[�h�̏������s���B
		Failed,		// ���s���s�B���[�g�m�[�h���琄�_���ĊJ����
		Complete	// ���s�����B���̃m�[�h�ɑJ��
	};

	BehaviorAction() = default;
	virtual ~BehaviorAction() = default;

	virtual void Initialize(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// ���̃m�[�h�ɑJ�ڂ����Ƃ��Ɏ��s����֐�
	/// </summary>
	/// <param name="actor"></param>
	virtual void Begin(const std::shared_ptr<Actor>& actor) {};

	/// <summary>
	/// ���s�����B�Ԃ����s���ɂ���ăc���[�̍s�����ω�����B
	/// <para>Run      �F���s���B���̃t���[���ɂ����Ă����̃m�[�h�̏������s���B</para>
	/// <para>Failed   �F���s���s�B���[�g�m�[�h���琄�_���ĊJ����</para>
	/// <para>Complete �F���s�����B���̃m�[�h�ɑJ��</para>
	/// </summary>
	virtual State Run(float elapsed_time, const std::shared_ptr<Actor>& actor) = 0;
};
