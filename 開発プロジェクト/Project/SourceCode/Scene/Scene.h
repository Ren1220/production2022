#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <d3d11.h>

#include "imgui.h"
#include "General/MyImGuiHelper.h"

/* --- 宣言 ---- */

class Scene
{
public:

	Scene()
		: is_ready(false), scene_timer() {}
	virtual ~Scene() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void EndProcess() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsed_time"></param>
	virtual void Update(float elapsed_time) = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Render(ID3D11DeviceContext* dc) = 0;


	/// <summary>
	/// ImGui描画処理
	/// </summary>
	virtual void ImGuiRender() {};

public:

	// 準備完了しているか
	bool IsReady() const { return is_ready; };

	// 準備完了設定
	void SetReady() { is_ready = true; }

protected:

	float scene_timer; // シーン内経過時間

	bool is_ready; // 初期化が完了したか
};