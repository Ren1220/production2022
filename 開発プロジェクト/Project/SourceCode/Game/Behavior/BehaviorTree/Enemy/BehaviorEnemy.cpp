#include "BehaviorEnemy.h"

/* --- �ǉ��̃w�b�_�t�@�C���̃C���N���[�h ---- */

#include "Game/General/GameSystem.h"
#include "Game/General/Title.h"
#include "Game/General/PlayerController.h"

#include "Game/Actor/Actor.h"


/* --- ��` ---- */

BehaviorEnemy::BehaviorEnemy()
	: is_damaged(false), player_vector(FLT_MAX, FLT_MAX, FLT_MAX)
{

}

BehaviorEnemy::~BehaviorEnemy()
{
}

void BehaviorEnemy::Start()
{
	BehaviorTree::Start();
}

void BehaviorEnemy::Update(float elapsed_time)
{
	// �Q�[���^�C�g���ł͓����Ȃ�
	if (TitleSystem::GetInstance().IsTitle()) return;

	// �v���C���[�Ƃ̋������v�����Ă���
	{
		player_vector =
			GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() -
				GetActor()->GetWorldPosition();
	}

	// ���s����m�[�h�����肷��
	BehaviorTree::Update(elapsed_time);
}

void BehaviorEnemy::ImGuiRender()
{
	BehaviorTree::ImGuiRender();
}
