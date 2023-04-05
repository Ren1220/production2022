#include "ColliderManager.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <algorithm>
#include <format>

#include "Debug/DebugGUI.h"

#include "Utility/Combination.h"

#include "../Actor/Actor.h"
#include "../General/GameSystem.h"
#include "../Physics/RigidBody.h"
#include "../Stage/StageManager.h"

#include "Collider.h"

/* --- 定義 ---- */

void ColliderManager::Update()
{
	// 参照先が破棄されたコライダーを除外する。
	RemoveExpiredCollider();
}

std::vector<std::shared_ptr<Actor>> ColliderManager::GetCollidedActorsWithSphere(
	const Vector3& sphere_position, 
	float sphere_radius, 
	const char* target_group_name)
{
	// 参照先が破棄されたコライダーを除外する。
	RemoveExpiredCollider();

	std::vector<std::shared_ptr<Actor>> collide_actors;

	// 領域の確保
	collide_actors.reserve(GameSystem::GetInstance().GetActorManager()->CountUpdateActors());

	for (const auto& wc : w_colliders)
	{
		auto collider{ wc.lock() };
		auto actor{ collider->GetActor() };

		// 実行可能か
		if (!collider->IsActive()) continue;

		// このコライダーを持つアクタのグループ名の判定
		if (!actor->CompareGroupName(target_group_name)) continue;
		
		// 一つのアクタが複数のコライダーを持つため、既に交差したアクタの処理を避ける
		if (std::ranges::find(collide_actors, actor) != collide_actors.end()) continue;

		// 当たり判定
		if (collider->IntersectSphere(sphere_position, sphere_radius))
		{
			// 交差した
			collide_actors.emplace_back(actor);
		}
	}

	// 不要な領域の縮小
	collide_actors.shrink_to_fit();

	return collide_actors;
}

void ColliderManager::OnHitEventActors()
{
	auto callback_function{ [&](const std::vector<std::shared_ptr<Actor>>& actors) -> void
	{
		auto colliders0 { actors[0]->GetColliders() };
		auto colliders1 { actors[1]->GetColliders() };

		const auto& actor0 { actors[0] };
		const auto& actor1 { actors[1] };

		for (const auto& c0 : colliders0) { c0.second->SetEventExecuted(false); }
		for (const auto& c1 : colliders1) { c1.second->SetEventExecuted(false); }

		bool is_event_executed{ false };

		// 二つのアクタの各コライダー同士で判定を行い交差していたらOnHitEvent()を実行しreturnする。
		for (const auto& c0 : colliders0)
		{
			const auto& collider0{ c0.second };
			// 交差判定を行うか
			if (!collider0->IsActive()) continue;

			for (const auto& c1 : colliders1)
			{
				const auto& collider1{ c1.second };

				// 交差判定を行うか
				if (!collider1->IsActive()) continue;

				float penetration{}; // 食い込み量

				if (collider0->Intersect(collider1, penetration))
				{
					// 交差している場合互いを引数にOnHitEvent()を実行する
					if (!is_event_executed)
					{
						actor0->OnHitEvent(actor1);
						actor1->OnHitEvent(actor0);
						is_event_executed = true;
					}

					// 既にこのコライダー固有のヒットイベントは呼ばれた
					if (!collider0->IsEventExecuted()) 
					{
						collider0->OnHitEventCallback(actor1);
						collider0->SetEventExecuted(true);
					}

					// 既にこのコライダー固有のヒットイベントは呼ばれた
					if (!collider1->IsEventExecuted())
					{
						collider1->OnHitEventCallback(actor0);
						collider1->SetEventExecuted(true);
					}

					auto rigid_body0{ actor0->GetRigidBody() };
					auto rigid_body1{ actor1->GetRigidBody() };

					// 互いに衝突可能なコライダーの時のみ
					if (collider0->IsIntersectOnly() || collider1->IsIntersectOnly())
					{
						continue;
					}

					if (rigid_body0 && rigid_body1)
					{
						rigid_body0->Collide(rigid_body1.get(), 0.01f, penetration);
					}
				}
			}
		}
	} };

	// アクタのコライダー同士の交差判定を行い、交差したアクタ同士のOnHitEvent()を実行する
	Combination::Execute<std::shared_ptr<Actor>>(
		GameSystem::GetInstance().GetActorManager()->GetUpdateActors(),  
		2,															    
		callback_function);											    
}

void ColliderManager::RemoveExpiredCollider()
{
	auto remove_range{ std::ranges::remove_if(w_colliders, [](const auto& c) { return c.expired(); }) };
	w_colliders.erase(remove_range.begin(), w_colliders.end());
}

void ColliderManager::RegisterCollider(
	const std::shared_ptr<Collider>& collider)
{
	w_colliders.emplace_back(collider);
}
