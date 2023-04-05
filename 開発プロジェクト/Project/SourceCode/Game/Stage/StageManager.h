#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <optional>

#include "Graphics/Shader/Shader.h"

#include "Stage.h"
#include "StageData.h"
#include "StageEditor.h"
#include "Terrain/TerrainRenderer.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

class StageManager
{
public:

	/// <summary>
	/// ここで設定したインデックスの階層のステージは別処理でステージを読み込み、生成する
	/// </summary>
	enum class ExtraStageIndex : uint32_t
	{
		Tutorial = 0U, // ゼロ階層目はチュートリアル
		Boss = 4U, //　まだボスは未実装
	};

	StageManager();
	~StageManager();

	void Initialize(ID3D11Device* device);
	void Update(float elapsed_time);
	void UpdateEditor(float elapsed_time);
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr);
	void EndProcess();
	void ImGuiRenderOnMainMenuBar();
	void ImGuiRender();

	void Retry();

	/// <summary>
	/// 引数のインデックスの階層に移動する。プレイヤーの位置を決定する。
	/// この階層のエンティティとして登録されたアクタはすべてアクタマネージャーから削除される。
	/// 初めて移動する階層の場合、地形生成を行い、生成した地形データをキャッシュデータとして出力する。
	/// 一度でも移動した階層の場合は、キャッシュデータから地形を読み込む。
	/// 広さ、奥行きを０に設定している場合 20~100 の範囲で各ランダムに設定される。
	/// </summary>
	void ChangeStage(
		uint32_t stage_width = 0U, 
		uint32_t stage_height = 0U,
		uint32_t enemy_appearance_amount = 5U,
		uint32_t item_appearance_amount = 5U);

	/// <summary>
	/// 生成された地形との当たり判定。
	/// 現状スフィアキャストでの判定。
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="sphere_cast_radius"></param>
	/// <returns></returns>
	bool IntersectModelVSTerrain(Actor* actor, float sphere_cast_radius);

	/// <summary>
	/// 引数のアクタの下の床に応じたイベントを実行する。
	/// フロア移動床をプレイヤが踏んだ場合trueを返す
	/// </summary>
	bool ExecuteFloorEvent();

	/// <summary>
	/// 引数のアクタの座標のX,Zを四捨五入した値を最寄りのタイルと判断してその値を返す。
	/// </summary>
	/// <param name="actor"></param>
	/// <returns>column, row</returns>
	std::pair<int64_t, int64_t> GetNearestIndex(Actor* actor);


	const Stage& GetStage() const { return stage; }

	uint32_t GetCurrentStageIndex() const  { return current_stage_index; }
	void	 SetNextStageIndex(uint32_t i) { next_stage_index = i;       }

	/// <summary>
	/// 次のステージのインデックスを指定し、ステージ遷移フラグを立てる。
	/// </summary>
	/// <param name="func">nullptrの場合、次のステージのインデックスは現在のインデックス＋１</param>
	/// <param name="fade_delta">nullptrの場合、次のステージのインデックスは現在のインデックス＋１</param>
	void NextStage(const std::function<uint32_t(uint32_t)>& func = nullptr, float fade_delta = 2.0f);
	
	/// <summary>
	/// 次のステージのインデックスを指定し、ステージ遷移フラグを立てる。
	/// </summary>
	/// <param name="i">次のステージのインデックスを決める</param>
	/// <param name="fade_delta">nullptrの場合、次のステージのインデックスは現在のインデックス＋１</param>
	void NextStage(uint32_t i, float fade_delta = 2.0f);
	
	/// <summary>
	/// ステージエディタの開始処理
	/// </summary>
	void StartEditor();

	/// <summary>
	/// ステージエディタの終了処理
	/// </summary>
	void EndEditor();

	// 現在のステージのアクタとして登録する。
	// ステージを移動したときにこのアクタは破棄される。
	// 敵を召喚したときなどに使う。
	void RegisterActor(const std::shared_ptr<Actor>& actor);

	void ActivateWarp();

private:

	void ChangeTutorialStage();

	void ChangeBossStage();

	void ChangeCommonStage(
		uint32_t stage_width,
		uint32_t stage_height,
		uint32_t enemy_appearance_amount,
		uint32_t item_appearance_amount);

	/// <summary>
	/// 引数のインデックスのステージのキャッシュが存在する場合、それを読み込み地形を生成する。
	/// </summary>
	/// <param name="stage_index"></param>
	/// <returns>キャッシュが存在しない場合　nullptr　を返す。</returns>
	bool ImportStage(uint32_t stage_index);

	/// <summary>
	/// 通常ステージのステージ生成データのキャッシュを破棄する。
	/// </summary>
	void RemoveStageCacheFile();

	/// <summary>
	/// レイとAABBによる衝突を行う
	/// </summary>
	/// <param name="rs">判定を行う縦のセルの始点</param>
	/// <param name="re">終点</param>
	/// <param name="cs">判定を行う横のセルの始点</param>
	/// <param name="ce">終点</param>
	/// <param name="map">地形マップ</param>
	/// <param name="ray_s"></param>
	/// <param name="ray_d"></param>
	/// <param name="ray_l"></param>
	/// <param name="out_norm"></param>
	/// <param name="out_near_len"></param>
	/// <returns>判定を行い衝突したらtrue</returns>
	bool CollisionTerrainByAABB(
		uint32_t rs, uint32_t re,
		uint32_t cs, uint32_t ce,
		const std::vector<std::vector<TerrainTile>>& map,
		const Vector3& ray_s,
		const Vector3& ray_d, float ray_l,
		Vector3& out_norm, float& out_near_len);

	/// <summary>
	/// モデルに対するスフィアキャストによる衝突を行う
	/// </summary>
	/// <param name="rs">判定を行う縦のセルの始点</param>
	/// <param name="re">終点</param>
	/// <param name="cs">判定を行う横のセルの始点</param>
	/// <param name="ce">終点</param>
	/// <param name="terrain"></param>
	/// <param name="ray_s"></param>
	/// <param name="ray_e"></param>
	/// <param name="sphere_radius">キャストする球の大きさ</param>
	/// <param name="out_intersect_pos"></param>
	/// <param name="out_wall_norm"></param>
	/// <returns></returns>
	bool CollisionTerrainBySphereCast(
		uint32_t rs, uint32_t re,
		uint32_t cs, uint32_t ce,
		const Terrain& terrain,
		const Vector3& ray_s,
		const Vector3& ray_e,
		float sphere_radius,
		Vector3& out_intersect_pos,
		Vector3& out_wall_norm);

	/// <summary>
	/// このステージのアクタとして生成されたオブジェクトを
	/// アクタ管理クラスから除外する。
	/// </summary>
	void RemoveCurrentStageActors();

private:

	std::optional<uint32_t> next_stage_index;
	uint32_t current_stage_index;
	Stage stage;
	TerrainRenderer terrain_renderer;
	Shader terrain_shader;

#ifdef _DEBUG
	StageEditor editor;
#endif // _DEBUG

};
