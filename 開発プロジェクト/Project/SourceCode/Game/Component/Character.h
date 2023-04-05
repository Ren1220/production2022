#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <optional>

#include "Component.h"

/* --- 宣言 ---- */

class Character : public Component
{
public:

	Character()
		: move_speed(1.0f), max_move_speed(5.0f), face_interpolation(1.0f), height(1.0f), radius(1.0f) {}
	~Character() override = default;

	/// <summary>
	/// 開始処理
	/// </summary>
	void Start() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;
	/// <summary>
	/// 交差時に実行する関数
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

	std::optional<Vector3> face_direction_goal; // 向こうとしている方向
	float face_interpolation;

	float max_move_speed;
	float move_speed;

	float height;
	float radius;

};
