#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <memory>

#include "../Component/Component.h"

/* --- �錾 ---- */

/// <summary>
/// Behavior�p�̊��N���X�B�ق�Component
/// ������p�����ăA�N�^�̍s���J�ڂ�����
/// </summary>
class Behavior : public Component
{
public:

	Behavior() = default;
	virtual ~Behavior() = default;
	virtual void Start() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void End() = 0;
	virtual void ImGuiRender() = 0;
};
