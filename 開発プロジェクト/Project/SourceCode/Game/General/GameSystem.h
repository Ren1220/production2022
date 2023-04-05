#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>
#include <memory>
#include <optional>

#include "Utility/Singleton.h"

/* --- 前方宣言 ---- */

class ActorManager;
class ColliderManager;
class ItemDataBase;
class PlayerController;
class Inventory;
class StageManager;
class HUDManager;
class GameText;

/* --- 宣言 ---- */

class GameSystem : public Singleton<GameSystem>
{
public:

	GameSystem();
	virtual ~GameSystem();

	void Initialize(ID3D11Device* device);
	void EndProcess();

	void Retry(ID3D11Device* device);

	void Update(float elapsed_time);

	const std::unique_ptr<ActorManager>&	 GetActorManager()		{ return actor_manager; }
	const std::unique_ptr<ColliderManager>&	 GetColliderManager()	{ return collider_manager; }
	const std::unique_ptr<ItemDataBase>&	 GetItemDataBase()		{ return item_data_base; }
	const std::unique_ptr<PlayerController>& GetPlayerController()	{ return player_controller; }
	const std::unique_ptr<Inventory>&		 GetInventory()			{ return inventory; }
	const std::unique_ptr<StageManager>&	 GetStageManager()		{ return stage_manager; }
	const std::unique_ptr<HUDManager>&		 GetHUDManager()		{ return hud_manager; }
	const std::unique_ptr<GameText>&		 GetGameText() { return game_text; }

	void ImGuiRenderOnMainMenu();

	void ImGuiRender();

	bool IsTransitionStage() const { return is_transition_stage; }
	void SetTransitionStageFlag(bool b) { is_transition_stage = b; }

	void GameStart();
	bool IsGameOver() const { return is_game_over; }
	void GameOver() { is_game_over = true; }

	void SetTimeLapseRatio(float ratio);
	void SetTimeLapseDelta(float delta, float dt);
	float GetTimeLapseRatio() const { return time_lapse_ratio; }

private:

	/// <summary>
	/// 初期配置のアクタ
	/// </summary>
	/// <param name="device"></param>
	void InitializeActor(ID3D11Device* device);

	std::unique_ptr<ActorManager>		actor_manager;
	std::unique_ptr<ColliderManager>	collider_manager;
	std::unique_ptr<ItemDataBase>		item_data_base;
	std::unique_ptr<PlayerController>	player_controller;
	std::unique_ptr<Inventory>			inventory;
	std::unique_ptr<StageManager>		stage_manager;
	std::unique_ptr<HUDManager>			hud_manager;
	std::unique_ptr<GameText>		    game_text;

#ifdef _DEBUG
	bool show_inventory_imgui;
	bool is_edit_mode;
#endif // _DEBUG

	bool is_game_over;
	bool is_transition_stage;

	// elapsed_timeの値のうちどれだけの割合を反映するか
	float time_lapse_ratio;
	// 一秒あたりの変化率
	float time_lapse_delta;
	float time_lapse_delta_t;

};


// マクロ
#define GameSys GameSystem::GetInstance()