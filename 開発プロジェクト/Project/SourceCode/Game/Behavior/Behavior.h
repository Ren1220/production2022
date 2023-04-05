#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>

#include "../Component/Component.h"

/* --- 宣言 ---- */

/// <summary>
/// Behavior用の基底クラス。ほぼComponent
/// これを継承してアクタの行動遷移を扱う
/// </summary>
class Behavior : public Component
{
public:

	Behavior() = default;
	virtual ~Behavior() = default;
	virtual void Start() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void End() = 0;
	virtual void ImGuiRender() = 0;
};
