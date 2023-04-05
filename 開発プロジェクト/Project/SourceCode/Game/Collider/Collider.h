#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <functional>
#include <unordered_map>

#include "Arithmetic/Arithmetic.h"

#include "../Component/Component.h"
#include "Graphics/Model/GLTFModelData.h"

/* --- 前方宣言 ---- */

class ColliderAABB;
class ColliderSphere;
class ColliderCapsule;

/* --- 宣言 ---- */

using TargetGroups = std::vector<std::string>;

class Collider: public Component
{
public:

	/// <summary>
	/// コライダーの種類
	/// </summary>
	enum class Type
	{
		AABB, Sphere, Capsule
	};

	enum class Target
	{
		None = 0x00000000,

	};

	Collider(const char* name, const Vector3& offset, const Vector4& color, bool is_intersect_only)
		: name(name), standard_point(), offset(offset), color(color), reference_node(), show_debug_primitive(false), is_intersect_only(is_intersect_only), is_event_executed(false)
	{
	}

	~Collider() override = default;


	/// <summary>
	/// 開始処理
	/// </summary>
	void Start() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	void Update(float elapsed_time) override;
	/// <summary>
	/// 他のアクタのコライダーと交差したときに実行される関数
	/// </summary>
	/// <param name="opponent_actor"></param>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender();

	/// <summary>
	/// このコライダーに設定されたコールバック関数を実行する。
	/// </summary>
	/// <param name="opponent"></param>
	void OnHitEventCallback(const std::shared_ptr<Actor>& opponent);

public:

	static Type GetColliderType(const size_t& hash_code);			// ハッシュ値のコライダーを取得する
	size_t GetHash() const { return typeid(*this).hash_code(); } 	// 自身のハッシュ値を取得する
	const Vector3& GetStandardPoint() const { return standard_point; }				// 基準の座標
	const Vector3& GetOffset() const { return offset; }				// 基準の座標からのオフセット

	void SetReferenceNode(const char* node_name);
	void SetOffset(const Vector3& o) { offset = o; }

	/// <summary>
	/// ハッシュリストが空のときIntersect()で呼ばれる
	/// </summary>
	static void InitializeHashList();
	/// <summary>
	/// 引数のコライダーとの交差判定
	/// </summary>
	bool Intersect(const std::shared_ptr<Collider>& opponent_collider);
	/// <summary>
	/// 引数のコライダーとの交差判定
	/// </summary>
	/// <param name="opponent_collider">判定を行う対象</param>
	/// <param name="out_normal">衝突した向きを格納</param>
	/// <param name="out_penetration">食い込み量を格納</param>
	/// <returns></returns>
	bool Intersect(const std::shared_ptr<Collider>& opponent_collider, float& out_penetration);

	virtual bool IntersectSphere(const Vector3& sphere_position, float sphere_radius) const = 0;

	/// <summary>
	/// 交差処理のみで衝突処理を行わない
	/// </summary>
	/// <returns></returns>
	bool IsIntersectOnly() const { return is_intersect_only; }
	void SetIntersectOnly(bool b) { is_intersect_only = b; }

	void SetShowDebugPrimitive(bool b) { show_debug_primitive = b; }

	/// <summary>
	/// OnHitEventで呼ばれるコールバック関数。一時的であったり、このコライダーのみの交差で関数を実行したいときに使う。
	/// </summary>
	/// <param name="func_name"></param>
	/// <param name="callback">引数は　自身のアクタ　と　交差したコライダーを持つアクタ</param>
	void AddOnHitEventCallBack(const char* func_name, const std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>& callback);
	void RemoveOnHitEventCallBack(const char* func_name);
	void ClearOnHitEventCallBack();

	bool IsEventExecuted() const { return is_event_executed; }
	void SetEventExecuted(bool b) { is_event_executed = b; }

private:

	/// <summary>
	/// AABBとの交差判定
	/// </summary>
	virtual bool IntersectAABB(const std::shared_ptr<ColliderAABB>& opponent, float& out_penetration) = 0;
	/// <summary>
	/// 球との交差判定
	/// </summary>
	virtual bool IntersectSphere(const std::shared_ptr<ColliderSphere>& opponent, float& out_penetration) = 0;
	///// <summary>
	///// 垂直円柱との交差判定
	///// </summary>
	//virtual bool IntersectVCylinder(const std::shared_ptr<ColliderVCylinder>& opponent, float& out_penetration) = 0;
	/// <summary>
	/// カプセルとの交差判定
	/// </summary>
	virtual bool IntersectCapsule(const std::shared_ptr<ColliderCapsule>& opponent, float& out_penetration) = 0;

private:

	static std::map<size_t, Type> collider_type_hash_list; // 各コライダーのハッシュ値

protected:

	std::string name;

	Vector3 standard_point;						 // 基準点
	Vector3 offset;								 // 基準の座標からのオフセット
	Vector4 color;								 // デバック描画する際の色
	std::weak_ptr<GLTFModelData::Node> reference_node; // 座標を参照するノードのアドレス

	bool show_debug_primitive;					 // デバッグ用プリミティブの描画を行うか
	bool is_intersect_only;						 // 交差判定のみ

	// OnHitEventで呼ばれるコールバック関数。一時的であったり、このコライダーのみの交差で関数を実行したいときに使う。
	// 引数は　自身のアクタ　と　交差したコライダーを持つアクタ
	std::unordered_map<std::string, std::function<void(const std::shared_ptr<Actor>&, const std::shared_ptr<Actor>&)>> on_hit_event_call_back;

	bool is_event_executed; // ColliderManagerで使うやつ
};
