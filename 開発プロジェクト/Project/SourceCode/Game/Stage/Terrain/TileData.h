#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <cstdint>

/* --- 前方宣言 ---- */


/* --- 宣言 ---- */

class TileData
{
public:

	TileData()
		: state(), mesh(), entity(), collide(), other(){}

	enum State : uint8_t
	{
		S_None = 0,
		S_PlayerSpawn, // フロア移動時にプレイヤーが配置される位置
		S_EnemySpawn,  // フロア移動時に敵が生成される位置
		S_ItemSpawn,   // フロア移動時にアイテムが生成される位置
		S_NextFloor,   // このタイルの上にプレイヤーがいると次のフロアに移動する
	};

	// 描画時に用いるメッシュ
	enum MeshID : uint8_t
	{
		M_None, 
		M_Floor,
		M_Wall,
		M_WallCeiling, // 上部のみの壁、描画負荷軽減のために周りが壁で描画する必要のない部分を無くしたメッシュ
	};

	// 出現する敵・アイテムのID
	enum EntityID : uint8_t
	{
		// 敵ID
		E_None,
		E_Random,   // 特殊な敵を除いてランダムで出現
		E_Skeleton,
		E_Wolf,
		E_Spider,

		E_SeparateExtra, // 特殊な敵との区切り
		E_Boss,
		E_End, 
		
		Separate = 127,// 敵とアイテムを分ける境界部

		// アイテムID
		I_Random,
		I_LifePot,
		I_FirePot,
		I_SparkPot,
		I_PoisonPot,
		I_End,

	};

	enum CollideFlag : uint8_t
	{
		C_None = 0,
		C_IsCollide    = 1 << 0,
		C_CollideUp    = 1 << 1,   // -Y
		C_CollideRight = 1 << 2,  // +X
		C_CollideDown  = 1 << 3,  // +Y
		C_CollideLeft  = 1 << 4,  // -X
	};
	
	enum OtherFlag : uint16_t
	{
		O_None = 0,
		O_EnemyHasWeapon = 1 << 0, // このタイルから出現する敵は武器を所持する（武器を持てる敵の場合のみ）
	};

	void AddCollideFlag(uint8_t rhs) { collide = static_cast<CollideFlag>(collide | rhs); }
	void AddOtherFlag(uint16_t  rhs) { other = static_cast<OtherFlag>(other| rhs); }

	State       state;
	MeshID      mesh;
	EntityID    entity;
	CollideFlag collide;
	OtherFlag   other;
};
