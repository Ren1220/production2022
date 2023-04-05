#pragma once

/* --- �w�b�_�t�@�C���̃C���N���[�h ---- */

#include <optional>

#include "Component.h"

/* --- �錾 ---- */

class Character : public Component
{
public:

	Character()
		: move_speed(1.0f), max_move_speed(5.0f), face_interpolation(1.0f), height(1.0f), radius(1.0f) {}
	~Character() override = default;

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

	void Move(const Vector3& direction, float face_interpolation);

	void SetMoveSpeed(float s) { move_speed = s; }
	void SetMaxMoveSpeed(float s) { max_move_speed = s; }
	void SetFaceDirection(const Vector3& direction, float interpolation);
	void SetRadius(float r) { radius = r; }
	void SetHeight(float h) { height = h; }

	float GetMoveSpeed() const { return move_speed; }
	float GetMaxMoveSpeed() const { return max_move_speed; }
	float GetRadius() const { return radius; }
	float GetHeight() const { return height; }

private: 

	void UpdateFaceDirection(float elapesd_time);

	std::optional<Vector3> face_direction_goal; // �������Ƃ��Ă������
	float face_interpolation;

	float max_move_speed;
	float move_speed;

	float height;
	float radius;

};
