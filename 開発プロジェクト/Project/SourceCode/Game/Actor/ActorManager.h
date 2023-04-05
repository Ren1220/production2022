#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>
#include <set>
#include <d3d11.h>

#include "Arithmetic/Arithmetic.h"
#include "Graphics/Shader/Shader.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

/// <summary>
/// Actorの管理クラス、基本的にこのクラスを介してアクタの生成を行う。
/// 生成されたアクタはまずStartActors配列に挿入され、Start()を実行した後、UpdateActors配列に移される。
/// 子アクタに関しては親アクタの更新処理内で更新が行われている。
/// RemoveActorsに同一のアクタが存在するアクタは終了処理を行った後UpdateActorsから除外される。
/// 描画処理はシェーダーの指定がない場合ジオメトリバッファに書き込みディファードレンダリングを行う。
/// </summary>
class ActorManager
{
public:

	void Initialize(ID3D11Device* device);

	/// <summary>
	/// 全アクタを解放し、そのコンポーネントの終了処理を実行する。
	/// </summary>
	void EndProcess();

	/// <summary>
	/// アクタを作成し、リストに登録する
	/// </summary>
	template <class T>
	std::shared_ptr<T> Create()
	{
		auto actor = std::make_shared<T>();
		{
			static int id = 0;
			char name[256];
			::sprintf_s(name, sizeof(name), "Actor%d", id++);
			actor->SetName(name);
		}
		start_actors.emplace_back(actor);
		return actor;
	}

	/// <summary>
	/// マネージャーの生成関数を利用せずに生成したアクタを登録する。
	/// 登録したアクタはStartActorsに挿入される。
	/// </summary>
	void Register(const std::shared_ptr<Actor>& actor);
	void Register(const std::vector<std::shared_ptr<Actor>>& actors);

	/// <summary>
	/// アクタを削除し、配列から除外する。
	/// 更新時にそのアクタの終了処理を実行し、破棄する。
	/// </summary>
	void Remove(const std::shared_ptr<Actor>& actor);
	void Remove(const std::vector<std::shared_ptr<Actor>>& actors);

	/// <summary>
	/// アクタの更新処理
	/// </summary>
	void UpdateActor(float elapsed_time);

	/// <summary>
	/// アクタのアニメーションの更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateAnimation(float elapsed_time);

	/// <summary>
	/// アクタのビヘイビアの更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateBehavior(float elapsed_time);

	/// <summary>
	/// アクタの剛体の更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateRigidBody(float elapsed_time);

	/// <summary>
	/// アクタのコライダーの更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateCollider(float elapsed_time);

	/// <summary>
	/// アクタの行列の更新
	/// </summary>
	void UpdateTransform(const Matrix& render_model_coordinate_space = Matrix::RhsYup());

	/// <summary>
	/// 現在管理下にあるアクタの再生中のアニメーションの実行可能時間にあるイベントを実行する
	/// </summary>
	void ExecuteActorCurrentPlayAnimationEvent();

	/// <summary>
	/// アクタの描画。外部のシェーダーを使わない場合Gbufferにモデル情報を書き込むシェーダーを設定する。
	/// </summary>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr);

	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender();

	void ImGuiRenderOnMainMenuBar();

	/// <summary>
	/// ハッシュ値を利用して引数名のアクタをリストから検索する
	/// </summary>
	std::shared_ptr<Actor> FindActor(const char* actor_name);

	/// <summary>
	/// ハッシュ値を利用して引数名のグループのアクタをリストから検索する
	/// </summary>
	std::vector<std::shared_ptr<Actor>> FindActors(const char* group_name);

	/// <summary>
	/// ハッシュ値を利用して引数名のグループのアクタをリストから検索する
	/// </summary>
	std::vector<std::shared_ptr<Actor>> FindActors(const std::vector<const char*>& group_names);

	/// <summary>
	/// UpdateActorsを取得
	/// </summary>
	const std::vector<std::shared_ptr<Actor>>& GetUpdateActors();

	size_t CountUpdateActors() const;

	void ClearAllActor();

private:

	/// <summary>
	/// start_actorsの処理。この処理の後update_actorsにポインタが移る。
	/// </summary>
	void UpdateStartActor();

	/// <summary>
	/// 各アクタの更新処理。子アクタの場合は親アクタの更新処理内部で処理を行う。
	/// </summary>
	/// <param name="elapsed_time"></param>
	void UpdateMainActor(float elapsed_time);

	/// <summary>
	/// Removeしたアクタの終了処理を行い、各リストから除外する。
	/// </summary>
	void RemoveActor();

	/// <summary>
	/// 各アクタの親子関係の更新
	/// </summary>
	void UpdateActorRelation();

	/// <summary>
	/// リスター
	/// </summary>
	void DrawLister();

	/// <summary>
	/// 詳細
	/// </summary>
	void DrawDetail();

private:

	std::vector<std::shared_ptr<Actor>> start_actors;	// Start()を呼び出すアクタ, Start()は一回だけ呼ばれる
	std::vector<std::shared_ptr<Actor>> update_actors;	// Update()を呼び出すアクタ
	std::set<std::shared_ptr<Actor>> selection_actors;	// 
	std::set<std::shared_ptr<Actor>> remove_actors;		// 

	bool show_lister = false;
	bool show_detail = false;
	bool show_animation_view = false;

	Shader gbuffer_shader;	// ジオメトリバッファに各情報を書き込むシェーダー
};
