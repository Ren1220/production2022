#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <optional>

#include "../Component.h"

/* --- �錾 ---- */

class Impact : public Component
{
public:

	Impact(float rising);
	~Impact() override = default;

	/// <summary>
	/// �J�n����
	/// </summary>
	void Start() override;
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui�`�揈��
	/// </summary>
	void ImGuiRender() override;
	/// <summary>
	/// �������Ɏ��s����֐�
	/// </summary>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;

private:
	
	float rising;
	float displacement;
	int state;

};
