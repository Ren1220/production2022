#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>

#include "Arithmetic/Arithmetic.h"

#include "Actor.h"

/* --- 宣言 ---- */

/// <summary>
/// 子アクタとして振舞う際に追加で必要なパラメータ
/// </summary>
class ChildActorParam
{
public:

	/// <summary>
	/// </summary>
	/// <param name="parent">このアクタの空間を基準にローカル座標を設定する。nullptrの時はワールド空間を基準にする。</param>
	ChildActorParam(const std::shared_ptr<Actor>& owner, const std::shared_ptr<Actor>& parent)
		: local_position(), local_rotation(), local_scale(), local_transform(),
		w_owner_actor(owner), w_parent_actor(parent), w_sibling_actors(),
		conform_position_only(false)
	{

	}

	void UpdateRelationToParent();

	/// <summary>
	/// 親アクタとの相互参照を解除する
	/// </summary>
	void UnlinkParentActor();

	std::shared_ptr<Actor> GetParentActor() const { return w_parent_actor.lock(); }

	const std::vector<std::weak_ptr<Actor>>& GetWSiblingActors(size_t index) const { return w_sibling_actors; };
	//std::shared_ptr<Actor> FindSiblingActor(const char* name);


	Vector3		          local_position;		// ローカル座標
	Quaternion	          local_rotation;		// ローカル回転
	Vector3		          local_scale;			// ローカルスケール
	Matrix		          local_transform;		// ローカル空間行列

	std::weak_ptr<Actor> w_owner_actor;	 // このパラメータを持つ子アクタ
	std::weak_ptr<Actor> w_parent_actor; // owner_actor の親アクタ
	std::vector<std::weak_ptr<Actor>> w_sibling_actors;

	bool conform_position_only;
};
