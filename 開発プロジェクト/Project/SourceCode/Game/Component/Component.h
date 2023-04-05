#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "imgui.h"

#include "Debug/DebugPrimitive.h"
#include "Debug/DebugGUI.h"
#include "General/MyImGuiHelper.h"

/* --- �O���錾 ---- */

class Actor;

/* --- �錾 ---- */

/// <summary>
/// �R���|�[�l���g���N���X
/// </summary>
class Component : public std::enable_shared_from_this<Component>
{
public:
	Component()
		: is_active(true), w_actor() {}
	virtual ~Component() = default;
	/// <summary>
	/// ���O�̎擾
	/// </summary>
	const char* GetName() const
	{
		// �p����ň�ł��֐����I�[�o�[���C�h���Ȃ��Ɛ������N���X�����擾�ł��Ȃ�
		return typeid(*this).name(); 
	}
	/// <summary>
	/// �J�n����
	/// </summary>
	virtual void Start()
	{

	}
	/// <summary>
	/// �I������
	/// </summary>
	virtual void End()
	{

	}
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	virtual void Update(float elapsed_time)
	{

	}
	/// <summary>
	/// �������Ɏ��s����֐�
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
	{

	}
	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	virtual void ImGuiRender()
	{

	}

	void SetActor(const std::shared_ptr<Actor>& actor) { this->w_actor = actor; } // �A�N�^�̎�Q��
	std::shared_ptr<Actor> GetActor() const;		  // �A�N�^�̎�Q��

	bool IsActive() const { return is_active; };
	void SetActiveFlag(bool set_active_flag) { is_active = set_active_flag; }

protected:

	std::weak_ptr<Actor> w_actor; // �A�N�^�̎�Q��

	bool is_active; // ���s�\�ȏ�Ԃ�

};
