#include "Stage.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <format>
#include <fstream>
#include <filesystem>

#include "Graphics/Model/GLTFStaticMesh.h"
#include "Graphics/Model/GLTFSkinnedMesh.h"
#include "General/AtaiEngine.h"
#include "Utility/RandomFunction.h"

#include "../Actor/Actor.h"
#include "../Actor/ActorManager.h"
#include "../Actor/ActorSkeleton.h"
#include "../Actor/ActorWolf.h"
#include "../Actor/ActorSpider.h"
#include "../Actor/ActorBoss.h"
#include "../Actor/ActorWeapon.h"
#include "../Actor/ActorItem.h"
#include "../Actor/Actor.h"
#include "../Component/Weapon/Weapon.h"
#include "../Component/Item/ItemFirePotion.h"
#include "../Component/Item/ItemLifePotion.h"
#include "../Component/Item/ItemPoisonPotion.h"
#include "../Component/Item/ItemSparkPotion.h"
#include "../General/GameSystem.h"
#include "../General/PlayerController.h"
#include "../HUD/HUDManager.h"


#include "StageData.h"
#include "Terrain/TerrainGenerator.h"

/* --- 定義 ---- */

void Stage::Initialize()
{
	stage_actors.clear();
}

void Stage::CreateTerrain(
	uint32_t map_width, 
	uint32_t map_height, 
	uint32_t enemy_appearance_amount, 
	uint32_t item_appearance_amount)
{
	this->item_appearance_amount = item_appearance_amount;
	this->enemy_appearance_amount = enemy_appearance_amount;
	
	// 初期化
	terrain.Initialize(map_width, map_height);

	constexpr uint32_t min_area_size       { 10U };
	constexpr uint32_t min_room_size       { 3U };
	constexpr uint32_t margin_between_area { 1U }; // 区域の境界と生成する部屋の最小間隔
	constexpr uint32_t room_wall_size	   { 2U }; 


	// 地形の生成
	TerrainGenerator::Generate(
		map_width, 
		map_height, 
		min_area_size,
		min_room_size,
		margin_between_area,
		room_wall_size,
		enemy_appearance_amount, 
		item_appearance_amount, 
		terrain);
}

void Stage::CreateTerrain(const char* filepath)
{
	// ファイルから読み込み
	terrain.Import(filepath);

	TerrainGenerator::UpdateTile(terrain);
}

void Stage::Enter(bool activate_warp)
{
	can_next = activate_warp;

	auto device{ MySystem.GetDevice() };
	const auto& actor_manager{ GameSystem::GetInstance().GetActorManager() };

	const auto& map{ terrain.GetMap() };
	uint32_t width{ terrain.GetMapWidth() };
	uint32_t height{ terrain.GetMapHeight() };

	// HUDミニマップの初期化
	GameSys.GetHUDManager()->GetMinimap().Reset();  

	auto enemy_map_indices{ terrain.FindMapIndices(TileData::S_EnemySpawn) };
	for (int ei = 0; const auto& [ci, ri] : enemy_map_indices)
	{
		const auto& tile{ map[ci][ri] };

		switch (map[ci][ri].data.entity)
		{
		case TileData::E_Random:
		{
			switch (Random::GetValue(static_cast<int>(TileData::E_Skeleton), (TileData::E_SeparateExtra - 1)))
			{
			case TileData::E_Skeleton:
			{
				std::string name{ std::format("EnemySkeleton{}", ei++) };
				auto create_actor{ actor_manager->Create<ActorSkeleton>() };
				create_actor->Initialize(
					device,
					name.c_str(),
					Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));

				// 生成したアクタをこのステージのアクタリストに登録する
				RegisterActor(create_actor);

				// 武器持ちか
//				if (tile.data.other & TileData::O_EnemyHasWeapon)
				{
					auto equip_weapon = create_actor->CreateChildActor<ActorWeapon>();
					equip_weapon->Initialize(
						device, std::format("EnemyWeapon{}", ei).c_str(), 10, 100, {});

					// 生成したアクタをこのステージのアクタリストに登録する
					RegisterActor(equip_weapon);
				}

				break;
			}
			case TileData::E_Wolf:
			{
				std::string name{ std::format("EnemyWolf{}", ei++) };
				auto create_actor{ actor_manager->Create<ActorWolf>() };
				create_actor->Initialize(
					device,
					name.c_str(),
					Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));

				// 生成したアクタをこのステージのアクタリストに登録する
				RegisterActor(create_actor);

				break;
			}
			case TileData::E_Spider:
			{
				std::string name{ std::format("EnemySpider{}", ei++) };
				auto create_actor{ actor_manager->Create<ActorSpider>() };
				create_actor->Initialize(
					device,
					name.c_str(),
					Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));

				// 生成したアクタをこのステージのアクタリストに登録する
				RegisterActor(create_actor);

				break;
			}
			}


			break;
		}
		case TileData::E_Skeleton:
		{
			std::string name{ std::format("EnemySkeleton{}", ei++) };
			auto create_actor{ actor_manager->Create<ActorSkeleton>() };
			create_actor->Initialize(
				device,
				name.c_str(),
				Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));

			// 生成したアクタをこのステージのアクタリストに登録する
			RegisterActor(create_actor);

			// 武器持ちか
//			if (tile.data.other & TileData::O_EnemyHasWeapon)
			{
				auto equip_weapon = create_actor->CreateChildActor<ActorWeapon>();
				equip_weapon->Initialize(
					device, std::format("EnemyWeapon{}", ei).c_str(), 10, 100, {});

				// 生成したアクタをこのステージのアクタリストに登録する
				RegisterActor(equip_weapon);
			}

			break;
		}
		case TileData::E_Wolf:
		{
			std::string name{ std::format("EnemyWolf{}", ei++) };
			auto create_actor{ actor_manager->Create<ActorWolf>() };
			create_actor->Initialize(
				device,
				name.c_str(),
				Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));
			// 生成したアクタをこのステージのアクタリストに登録する
			RegisterActor(create_actor);

			break;
		}
		case TileData::E_Spider:
		{
			std::string name{ std::format("EnemySpider{}", ei++) };
			auto create_actor{ actor_manager->Create<ActorSpider>() };
			create_actor->Initialize(
				device,
				name.c_str(),
				Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));
			// 生成したアクタをこのステージのアクタリストに登録する
			RegisterActor(create_actor);

			break;
		}
		case TileData::E_Boss:
		{
			std::string name{ std::format("EnemyBoss") };
			auto create_actor{ actor_manager->Create<ActorBoss>() };
			create_actor->Initialize(
				device,
				name.c_str(),
				Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));
			// 生成したアクタをこのステージのアクタリストに登録する
			RegisterActor(create_actor);

			break;
		}
		}
	}

	auto item_map_indices{ terrain.FindMapIndices(TileData::S_ItemSpawn) };
	for (int ii = 0; const auto& [ci, ri] : item_map_indices)
	{
		std::string name{ std::format("Item{}", ii++) };
		auto create_actor{ actor_manager->Create<ActorItem>() };

		create_actor->Initialize(
			device, 
			name.c_str(),
			Vector3(static_cast<float>(ci), 0.3f, static_cast<float>(ri)));

		// 生成したアクタをこのステージのアクタリストに登録する
		RegisterActor(create_actor);

		ItemDataBase::List list{  };
		switch (map[ci][ri].data.entity)
		{
		case TileData::I_LifePot:    list = ItemDataBase::List::LifePotion; break;
		case TileData::I_FirePot:    list = ItemDataBase::List::FirePotion; break;
		case TileData::I_PoisonPot:  list = ItemDataBase::List::PoisonPotion; break;
		case TileData::I_SparkPot:   list = ItemDataBase::List::SparkPotion; break;
		case TileData::I_Random:
		default:
			list = static_cast<ItemDataBase::List>(Random::GetValue(0, static_cast<int>(ItemDataBase::List::ElementCount) - 1));
			break;
		}

		// 所都合で現状毒ポーションはなし、再抽選を行う
		while (list == ItemDataBase::List::PoisonPotion)
		{
			list = static_cast<ItemDataBase::List>(Random::GetValue(0, static_cast<int>(ItemDataBase::List::ElementCount) - 1));
		}

		switch (list)
		{
		case ItemDataBase::List::LifePotion:
			create_actor->AddComponent<ItemLifePotion>(100);
			break;
		case ItemDataBase::List::FirePotion:
			create_actor->AddComponent<ItemFirePotion>();
			break;
		case ItemDataBase::List::PoisonPotion:
			create_actor->AddComponent<ItemPoisonPotion>(50);
			break;
		case ItemDataBase::List::SparkPotion:
			create_actor->AddComponent<ItemSparkPotion>();
			break;
		default:
			break;
		}
	}

	// 次のフロアへ移動するための床
	warp_effect.Destroy();
	if (activate_warp)
	{
		auto warp_map_indices{ terrain.FindMapIndices(TileData::S_NextFloor) };
		for (int wi = 0; const auto & [ci, ri] : warp_map_indices)
		{
			std::lock_guard<std::mutex> lock(MySystem.GetMutex());

			warp_effect.Initialize("Warp/warp2.efk", Vector3(static_cast<float>(ci), 0.01f, static_cast<float>(ri)));
		}
	}

	// プレイヤーの位置を決定する。
	{
		auto [x, y] { terrain.FindMapIndices(TileData::S_PlayerSpawn).front() };
		auto player{ GameSys.GetPlayerController()->GetPlayer() };

		player->SetWorldPosition({ static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}
}

void Stage::Exit()
{
}

void Stage::ExportStageData(uint32_t index)
{
	StageData export_data;
	
	// 出力
	std::string filename{ std::format("Data/StageData/stage{}.bin", index) };
	export_data.Export(filename.c_str(), terrain);
}

void Stage::ImGuiRender()
{
	const char8_t* text { u8"フロアのサイズ : %d×%d\n敵の出現数 : %d\nアイテムの出現数 : %d" };
	ImGui::Text(TO_CHAR(text), terrain.GetMapWidth(), terrain.GetMapHeight(), enemy_appearance_amount, item_appearance_amount);
}

void Stage::RegisterActor(const std::shared_ptr<Actor>& actor)
{
	stage_actors.emplace_back(actor);
}

void Stage::ActivateWarp()
{
	can_next = true;

	// 次のフロアへ移動するための床
	auto warp_map_indices{ terrain.FindMapIndices(TileData::S_NextFloor) };
	for (int wi = 0; const auto & [ci, ri] : warp_map_indices)
	{
		std::lock_guard<std::mutex> lock(MySystem.GetMutex());

		warp_effect.Destroy();
		warp_effect.Initialize("Warp/warp2.efk", Vector3(static_cast<float>(ci), 0.01f, static_cast<float>(ri)));
	}
}
