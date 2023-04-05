#include "Character.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <DirectXMath.h>

#include "../Actor/Actor.h"
#include "../Physics/RigidBody.h"

/* --- 定義 ---- */

void Character::Start()
{
}

void Character::Update(float elapsed_time)
{
	UpdateFaceDirection(elapsed_time);
}

void Character::ImGuiRender()
{

}

void Character::OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
{
}

void Character::Move(const Vector3& direction, float face_interpolation)
{
	if (direction.IsZero()) return;
	auto rigid_body{ GetActor()->GetRigidBody() };

	rigid_body->AddMoveDirectionForce(direction, move_speed, max_move_speed);
	SetFaceDirection(direction, face_interpolation);
}

void Character::SetFaceDirection(const Vector3& direction, float interpolation)
{
	face_direction_goal = direction.Normalize();
	face_interpolation = interpolation;	
}

void Character::UpdateFaceDirection(float elapesd_time)
{
	// 向きたい方向を設定していない(既に向いていると扱う)場合は処理を行わない。
	if (face_direction_goal.has_value() == false) return;

	auto actor{ GetActor() };

	// 向きたい方向
	const Vector3& goal{ face_direction_goal.value() };

	// 現在のアクタの方向ベクトル
	Quaternion rot{ actor->GetWorldRotation() };
	Matrix rm{ Matrix::CreateRotation(rot) };
	Vector3 right{ rm.m[0] };
	Vector3 up{ rm.m[1] };
	Vector3 forward{ rm.m[2] };

	float dot{ forward.Dot(goal) };

	// 目標の向きと一致している場合は処理を行わない
	if (dot > 1.0f - FLT_EPSILON)
	{
		// 空にする。
		face_direction_goal.reset();
		if (actor->CompareGroupName("Player"))
		{
			DebugGUI::GetInstance().SetString("None");
		}
		return;
	}

	// 値がおかしくならないように制限をかける
	dot = std::clamp(dot, -1.0f + FLT_EPSILON, 1.0f - FLT_EPSILON);

	// 求めた内積から角度を求める
	float angle{ acosf(dot) };

	// 外積を行い、回転方向を判断する
	if (forward.Cross(goal).y < 0.0f)
	{
		angle = -angle;
	}

	//// 目標の向きとの外積と上ベクトルの内積のなす角が90度を超える場合は逆回転
	//if (up.Dot(forward.Cross(goal)) < 0)
	//{
	//	angle = -angle;
	//}

	// 回転量を求め設定する
	//actor->SetWorldRotation(rot * Quaternion(up, angle).Normalize());
	Quaternion result{ rot.Slerp(rot * Quaternion(up, angle), face_interpolation * elapesd_time) };
	actor->SetWorldRotation(result);

}
