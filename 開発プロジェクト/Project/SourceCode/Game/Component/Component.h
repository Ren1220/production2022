#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "imgui.h"

#include "Debug/DebugPrimitive.h"
#include "Debug/DebugGUI.h"
#include "General/MyImGuiHelper.h"

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

/// <summary>
/// コンポーネント基底クラス
/// </summary>
class Component : public std::enable_shared_from_this<Component>
{
public:
	Component()
		: is_active(true), w_actor() {}
	virtual ~Component() = default;
	/// <summary>
	/// 名前の取得
	/// </summary>
	const char* GetName() const
	{
		// 継承先で一つでも関数をオーバーライドしないと正しくクラス名を取得できない
		return typeid(*this).name(); 
	}
	/// <summary>
	/// 開始処理
	/// </summary>
	virtual void Start()
	{

	}
	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End()
	{

	}
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time">経過時間</param>
	virtual void Update(float elapsed_time)
	{

	}
	/// <summary>
	/// 交差時に実行する関数
	/// </summary>
	virtual void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor)
	{

	}
	/// <summary>
	/// ImGui描画処理
	/// </summary>
	virtual void ImGuiRender()
	{

	}

	void SetActor(const std::shared_ptr<Actor>& actor) { this->w_actor = actor; } // アクタの弱参照
	std::shared_ptr<Actor> GetActor() const;		  // アクタの弱参照

	bool IsActive() const { return is_active; };
	void SetActiveFlag(bool set_active_flag) { is_active = set_active_flag; }

protected:

	std::weak_ptr<Actor> w_actor; // アクタの弱参照

	bool is_active; // 実行能な状態か

};
