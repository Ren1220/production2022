#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>

#include "Terrain/Terrain.h"
#include "Graphics/Effekseer/EffekseerHandle.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class Stage
{
public:

	Stage() = default;
	virtual ~Stage() = default;

	virtual void Initialize();

	virtual void CreateTerrain(
		uint32_t stage_width, 
		uint32_t stage_height,
		uint32_t enemy_appearance_amount,
		uint32_t item_appearance_amount);

	virtual void CreateTerrain(const char* filepath);

	/// <summary>
	/// このステージに入場した際に行う処理
	/// </summary>
	virtual void Enter(bool activate_warp = true);

	/// <summary>
	/// このステージを退場した際に行う処理
	/// </summary>
	virtual void Exit();

	void ExportStageData(uint32_t index);
	
	void ImGuiRender();

	std::pair<uint32_t, uint32_t> GetStageSize() const { return { terrain.GetMapWidth(), terrain.GetMapHeight() }; }
	const Terrain& GetTerrain() const { return terrain; }
	Terrain& GetTerrain() { return terrain; }

	const std::vector<std::shared_ptr<Actor>>& GetStageActors() const { return stage_actors;  
	}

	
	/// <summary>
	/// 引数のアクタをこのフロアのアクタとして配列に登録する。
	/// ステージを切り替える際に、登録したアクタを削除するのに使う。
	/// </summary>
	void RegisterActor(const std::shared_ptr<Actor>& actor);

	void ActivateWarp();

	bool CanNext() const { return can_next; }

protected:

	uint32_t enemy_appearance_amount;
	uint32_t item_appearance_amount;

	// このステージで生成したアクタ
	std::vector<std::shared_ptr<Actor>> stage_actors;

	// 地形情報
	Terrain terrain;

	EffekseerHandle warp_effect;

	bool can_next;
};
