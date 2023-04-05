#pragma once

/* --- 概要 ---- */

// 基本的な部分の説明
// 
//   Actorに様々な機能を追加するComponent配列を持たせ、ActorManagerで管理することで、
//   ゲーム内に登場するオブジェクトの操作を各イベントごとに実行することができ、
//   これにより、更新する順番による不具合を減らすことができる。
//   Componentはこれを基底クラスとして継承し、様々な機能を追加する。
//   個別に参照する際にdynamic_castを行う。
// 
// 追加した部分の説明
// 
// ・アクタ同士の親子関係
// 　再起処理を行い、親アクタの変換行列から子アクタのワールド座標を算出している。
// 　メモリの消費を抑えるため、子アクタ時のパラメータは外部クラスにて管理されている。
// 
// ・Behavior/Animator/Collider/RigidBody機能
//　 これらは必要に応じて任意で追加する機能
//	 Component配列とは別で実体をもたせている。
// 　これらの機能をComopnentの更新とは別で行わないと不具合が発生するため分離。
//   具体的にはBehaviorであったらComponent内のパラメータを利用して、
//   次のふるまいを決定する可能性がある等。
// 
//   Behavior : BehaviorTreeもしくはStateMachineによる行動遷移の管理。
// 
//   Animator : アクタのもつSkinnedMeshのアニメーションと
// 　　　　　   各アニメーションに設定した任意のフレームでコールバック関数を呼び出す
//              AnimationEventの管理。
// 
//   RigidBody : アクタの速力処理。
// 
//   Collider : アクタ同士の衝突（交差）判定と衝突（交差）時に実行する
//              Component内の関数のOnHitEventの実行と、
//              RigidBodyを利用したアクタ同士の押し出しを行う。
// 
//   各機能の詳細はそのクラスにて説明。
// 
// ・std::hashを利用したハッシュ値による名前検索機能による高速化。
//

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <optional>
#include <cassert>

#include "imgui.h"

#include "ActorManager.h"
#include "Arithmetic/Arithmetic.h"
#include "Game/General/GameSystem.h"
#include "General/MyImGuiHelper.h"
#include "Graphics/Model/GLTFModel.h"

/* --- 前方宣言 ---- */

class ChildActorParam;
class Animator;
class Component;
class Behavior;
class Collider;
class RigidBody;

/* --- 定義 ---- */

class Actor : public std::enable_shared_from_this<Actor>
{
public:

	Actor();
	virtual ~Actor();

	/// <summary>
	/// 子アクタの場合は、localのパラメータを設定する。
	/// </summary>
	virtual void Initialize(
		const char* name,
		const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale,
		const char* group_name);

	/// <summary>
	/// 子アクタの場合は、localのパラメータを設定する
	/// </summary>
	/// <typeparam name="T">設定するモデルのクラス</typeparam>
	template <class T>
	void Initialize(
		ID3D11Device* device,
		const char* name,
		const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale,
		const char* group_name,
		const char* model_file_path);

	/// <summary>
	/// ActorManagerのStartActorで呼び出される処理
	/// </summary>
	virtual void Start();

	/// <summary>
	/// ActorManagerのUpdateActorで呼び出される処理。
	/// 子アクタの場合は親アクタのUpdate内で再帰的に更新処理を行う。
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Update(float elapsed_time);

	/// <summary>
	/// ActorManagerのRemoveActorで呼び出される処理
	/// </summary>
	virtual void End();

	/// <summary>
	/// ActorManagerのUpdateTransformで呼び出される処理。
	/// 子アクタの場合は親アクタの関数内で再帰的に更新処理を行う。
	/// </summary>
	/// <param name="render_model_coordinate_space">モデルの座標系</param>
	virtual void UpdateTransform(const Matrix& render_model_coordinate_space = Matrix::RhsYup());

	/// <summary>
	/// 次フレームにおいての変換行列の設定。
	/// 子アクタの場合はローカル空間の設定を行う。(これに親アクタのワールド変換行列をかけてワールド変換行列を計算する。)
	/// </summary>
	/// <param name="transform"></param>
	void SetNextTransform(const Matrix& transform)
	{
		next_transform = transform;
	}

	/// <summary>
	/// 現在のローカル座標系をワールド座標系に置き換える。
	/// 親子関係のあるアクタの関係を切った際にアクタの座標がおかしくならないようにするために使う。
	/// </summary>
	void SetLocalTransformToWorld()
	{
		next_transform = world_transform;
	}

	/// <summary>
	/// 交差時に実行される各コンポーネントの関数を実行する
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) const;

	/// <summary>
	/// 現在のアニメーション時間に応じた再生中のアニメーションのイベントを実行する
	/// </summary>
	void ExecuteCurrentPlayAnimationEvent();

	void SetName(const char* name) { this->name = name; }
	void SetGroupName(const char* group_name) { this->group_name = group_name; }
	void SetWorldPosition(const Vector3& position) { this->world_position = position; }
	void SetWorldRotation(const Quaternion& rotation) { this->world_rotation = rotation; }
	void SetWorldScale(const Vector3& scale) { this->world_scale = scale; }

	const std::string& GetName()		   const { return name; }
	const std::string& GetGroupName()	   const { return group_name; }
	const Vector3& GetWorldPosition()  const { return world_position; }
	const Quaternion& GetWorldRotation()  const { return world_rotation; }
	const Vector3& GetWorldScale()     const { return world_scale; }
	const Matrix& GetWorldTransform() const { return world_transform; }

	/// <summary>
	/// ハッシュ値による名前の比較
	/// </summary>
	bool CompareName(const char* opponent) const;

	/// <summary>
	/// ハッシュ値による名前の比較
	/// </summary>
	bool CompareGroupName(const char* opponent) const;

protected:

	/// <summary>
	/// 自身が子アクタの場合の行列更新処理。その子アクタの行列更新も再帰的に行う。
	/// </summary>
	/// <param name="parent_coordinate"></param>
	void UpdateTransformAsChild(const Matrix& parent_coordinate);

	std::string           name;					// 名前
	std::string           group_name;			// グループ名同じ種類のアクタを一括で処理する際に使う
	Vector3		          world_position;		// ワールド座標
	Quaternion	          world_rotation;		// ワールド回転
	Vector3		          world_scale;			// ワールドスケール
	Matrix		          world_transform;		// ワールド空間変換行列

	// 上記のパラメータを利用せずに座標を設定する場合に利用する。
	// 回転行列を一度クォータニオンに変換して、
	// 再度回転行列にした際に回転量で不具合が発生したためその対策で使う場合がある。
	std::optional<Matrix> next_transform;		

public: // 描画関係

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	template <class T>
	void CreateModel(ID3D11Device* device, const char* filepath)
	{
		model = std::make_unique<T>(device, filepath);
	}

	template <class T>
	T* GetModel()
	{
		return dynamic_cast<T*>(model.get());
	}

	GLTFModel* GetModel() { return model.get(); }

protected:

	std::unique_ptr<GLTFModel> model; // モデル

public:

	void UpdateBehavior(float elapsed_time) const;
	void UpdateAnimation(float elapsed_time) const;
	void UpdateRigidBody(float elapsed_time) const;
	void UpdateCollider(float elapsed_time) const;


	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <typeparam name="T">追加するコンポーネント</typeparam>
	/// <typeparam name="...Args">コンポーネントクラスコンストラクタ引数</typeparam>
	/// <returns>追加したコンポーネントを返す</returns>
	template <class T, class... Args>
	std::shared_ptr<T> AddComponent(Args ... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		components.emplace_back(component);
		return component;
	}

	/// <summary>
	/// ビヘイビアの作成
	/// </summary>
	/// <typeparam name="T">追加するビヘイビア</typeparam>
	/// <typeparam name="...Args">ビヘイビアクラスコンストラクタ引数</typeparam>
	/// <returns>追加したビヘイビアを返す</returns>
	template <class T, class... Args>
	std::shared_ptr<T> CreateBehavior(Args ... args)
	{
		std::shared_ptr<T> ptr = std::make_shared<T>(args...);
		ptr->SetActor(shared_from_this());
		behavior = ptr;
		return ptr;
	}

	/// <summary>
	/// アニメーションクラスの作成。生成するモデルはSkinnedMeshでないと使えない。
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Animator> CreateAnimator();

	/// <summary>
	/// 剛体処理を行うオブジェクトを生成する。
	/// </summary>
	/// <typeparam name="mass">質量</typeparam>
	/// <typeparam name="is_effect_gravity">重力の影響下にあるか</typeparam>
	std::shared_ptr<RigidBody> CreateRigidBody(float mass, bool is_effect_gravity);

	/// <summary>
	/// 交差判定を行うコライダーを作成し、配列に追加する。
	/// 複数設定が可能。
	/// </summary>
	/// <typeparam name="T">追加するコライダー</typeparam>
	/// <typeparam name="...Args">ビヘイビアクラスコンストラクタ引数</typeparam>
	/// <param name="name">追加したコライダーの名前</param>
	/// <returns></returns>
	template <class T, class... Args>
	std::shared_ptr<T> CreateCollider(const char* name, Args... args)
	{
		std::shared_ptr<T> ptr = std::make_shared<T>(name, args...);
		ptr->SetActor(shared_from_this());
		RegisterCollider(ptr);
		colliders[name] = ptr;
		return ptr;
	}


	/// <summary>
	/// コンポーネント配列のコンポーネントをTに一つずつダウンキャストして
	/// 一致する場合そのポインタを返す
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		// 保持しているコンポーネントを指定の型にキャストして、一致した場合はそのコンポーネントを返す
		for (std::shared_ptr<Component>& component : components)
		{
			// コンポーネントが発見された
			if (std::shared_ptr<T> found_component = std::dynamic_pointer_cast<T>(component))
			{
				return found_component;
			}
		}

		return nullptr;
	}

	template <class T>
	std::shared_ptr<T> GetBehavior()
	{
		assert(behavior != nullptr);
		return std::dynamic_pointer_cast<T>(behavior);
	}

	std::shared_ptr<Collider> GetCollider(const char* name)
	{
		auto itr{ colliders.find(name) };
		if (itr != colliders.end())
		{
			return itr->second;
		}

		return nullptr;
	}

	template <class T>
	std::shared_ptr<T> GetCollider(const char* name)
	{
		if (std::shared_ptr<T> collider = std::dynamic_pointer_cast<T>(GetCollider(name)))
		{
			return collider;
		}

		return nullptr;
	}


	std::shared_ptr<Animator> GetAnimator();
	std::shared_ptr<RigidBody> GetRigidBody();
	const std::vector<std::shared_ptr<Component>>& GetComponents() const { return components; }
	const std::unordered_map<std::string, std::shared_ptr<Collider>>& GetColliders() const;

private:

	/// <summary>
	/// 生成したコライダーをコライダー管理クラスに登録する。
	/// 登録することで管理クラスを介した衝突判定、処理を行うことができる。
	/// </summary>
	/// <param name="collider"></param>
	void RegisterCollider(const std::shared_ptr<Collider>& collider);

	std::vector<std::shared_ptr<Component>> components;
	std::shared_ptr<Behavior> behavior;
	std::shared_ptr<Animator> animator;
	std::shared_ptr<RigidBody> rigid_body;
	std::unordered_map<std::string, std::shared_ptr<Collider>> colliders;

public: // アクタの親子関係

	/// <summary>
	/// 参照が切れた関係を持つアクタをリストから削除する
	/// </summary>
	void UpdateActorRelation();

	/// <summary>
	/// このアクタの子アクタとして新しいインスタンスを作成する。
	/// 作成するにおいて、親子同士、親アクタから見ての兄弟同士の弱参照が登録される。
	/// </summary>
	/// <returns></returns>
	template <class T>
	std::shared_ptr<T> CreateChildActor()
	{
		auto created_actor{ GameSystem::GetInstance().GetActorManager()->Create<T>() };

		AddChildActor(created_actor);

		created_actor->SetParentActor(shared_from_this());

		return created_actor;
	}

	void RemoveChildActor(const std::shared_ptr<Actor>& actor);
	/// <summary>
	/// 同時に既に子アクタとして登録しているほかのアクタの兄弟としても登録する
	/// </summary>
	void AddChildActor(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// 引数に null_ptr を設定した場合、親アクタが存在しない状態になる
	/// </summary>
	void SetParentActor(const std::shared_ptr<Actor>& actor);

	std::shared_ptr<Actor> GetChildActor(size_t index) const { return w_child_actors.at(index).lock(); }
	size_t CountOfChildActors() const { return w_child_actors.size(); }

	bool IsChildActor() const { return is_child; }
	void SetChildActorFlag(bool is_child) { this->is_child = is_child; }

	const std::unique_ptr<ChildActorParam>& GetChildActorParam() { return child_actor_param; }

private:

	bool is_child; // 子アクタか
	// 子アクタとして振舞う際に用いるパラメータ
	std::unique_ptr<ChildActorParam> child_actor_param;

	std::vector<std::weak_ptr<Actor>> w_child_actors;
	std::set<std::shared_ptr<Actor>> remove_child_actors;

public: // デバック

	void ImGuiRender();

};

template<class T>
inline void Actor::Initialize(
	ID3D11Device* device,
	const char* name, 
	const Vector3& position, 
	const Quaternion& rotation, 
	const Vector3& scale, 
	const char* group_name, 
	const char* model_file_path)
{
	Initialize(name, position, rotation, scale, group_name);
	CreateModel<T>(device, model_file_path);
}