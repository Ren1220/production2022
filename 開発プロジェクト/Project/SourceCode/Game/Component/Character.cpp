#include "Character.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include <DirectXMath.h>

#include "../Actor/Actor.h"
#include "../Physics/RigidBody.h"

/* --- ��` ---- */

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
	// ��������������ݒ肵�Ă��Ȃ�(���Ɍ����Ă���ƈ���)�ꍇ�͏������s��Ȃ��B
	if (face_direction_goal.has_value() == false) return;

	auto actor{ GetActor() };

	// ������������
	const Vector3& goal{ face_direction_goal.value() };

	// ���݂̃A�N�^�̕����x�N�g��
	Quaternion rot{ actor->GetWorldRotation() };
	Matrix rm{ Matrix::CreateRotation(rot) };
	Vector3 right{ rm.m[0] };
	Vector3 up{ rm.m[1] };
	Vector3 forward{ rm.m[2] };

	float dot{ forward.Dot(goal) };

	// �ڕW�̌����ƈ�v���Ă���ꍇ�͏������s��Ȃ�
	if (dot > 1.0f - FLT_EPSILON)
	{
		// ��ɂ���B
		face_direction_goal.reset();
		if (actor->CompareGroupName("Player"))
		{
			DebugGUI::GetInstance().SetString("None");
		}
		return;
	}

	// �l�����������Ȃ�Ȃ��悤�ɐ�����������
	dot = std::clamp(dot, -1.0f + FLT_EPSILON, 1.0f - FLT_EPSILON);

	// ���߂����ς���p�x�����߂�
	float angle{ acosf(dot) };

	// �O�ς��s���A��]�����𔻒f����
	if (forward.Cross(goal).y < 0.0f)
	{
		angle = -angle;
	}

	//// �ڕW�̌����Ƃ̊O�ςƏ�x�N�g���̓��ς̂Ȃ��p��90�x�𒴂���ꍇ�͋t��]
	//if (up.Dot(forward.Cross(goal)) < 0)
	//{
	//	angle = -angle;
	//}

	// ��]�ʂ����ߐݒ肷��
	//actor->SetWorldRotation(rot * Quaternion(up, angle).Normalize());
	Quaternion result{ rot.Slerp(rot * Quaternion(up, angle), face_interpolation * elapesd_time) };
	actor->SetWorldRotation(result);

}
