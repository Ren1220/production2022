#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <optional>

#include "../Component.h"

/* --- 宣言 ---- */

class Impact : public Component
{
public:

	Impact(float rising);
	~Impact() override = default;

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
	/// ImGui描画処理
	/// </summary>
	void ImGuiRender() override;
	/// <summary>
	/// 交差時に実行する関数
	/// </summary>
	void OnHitEvent(const std::shared_ptr<Actor>& opponent_actor) override;

private:
	
	float rising;
	float displacement;
	int state;

};
