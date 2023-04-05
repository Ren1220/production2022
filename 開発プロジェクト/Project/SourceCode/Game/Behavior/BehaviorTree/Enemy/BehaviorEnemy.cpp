#include "BehaviorEnemy.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Game/General/GameSystem.h"
#include "Game/General/Title.h"
#include "Game/General/PlayerController.h"

#include "Game/Actor/Actor.h"


/* --- 定義 ---- */

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
	// ゲームタイトルでは動けない
	if (TitleSystem::GetInstance().IsTitle()) return;

	// プレイヤーとの距離を計測しておく
	{
		player_vector =
			GameSys.GetPlayerController()->GetPlayer()->GetWorldPosition() -
				GetActor()->GetWorldPosition();
	}

	// 実行するノードを決定する
	BehaviorTree::Update(elapsed_time);
}

void BehaviorEnemy::ImGuiRender()
{
	BehaviorTree::ImGuiRender();
}
