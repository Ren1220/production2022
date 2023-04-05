#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <functional>
#include <memory>
#include <vector>

/* --- 前方宣言 ---- */

class Actor;

/* --- 宣言 ---- */

/// <summary>
/// アニメーションイベント
/// <para>Animatorコンポーネントでこのインスタンスを登録し、実行可能なコールバック関数を実行する</para>
/// </summary>
class AnimationCallbackEvent 
{
public:

	AnimationCallbackEvent(const std::function<void(const std::shared_ptr<Actor>&)>& event_function);
	virtual ~AnimationCallbackEvent() = default;

	/// <summary>
	/// ループ毎に行う初期化処理
	/// </summary>
	virtual void InitializePerLoop() {};

	/// <summary>
	/// イベントが実行可能か
	/// </summary>
	virtual bool CanExecuteEvent(float time) const { return false; }

	/// <summary>
	/// イベントの実行
	/// </summary>
	virtual void Execute(const std::shared_ptr<Actor>& actor);

protected:

	std::function<void(const std::shared_ptr<Actor>&)> event_function;
};

/// <summary>
/// ループ毎に指定時間以降一度だけ実行されるイベント
/// </summary>
class AnimationEventOncePerLoop : public AnimationCallbackEvent
{
public:
	/// <summary></summary>
	/// <param name="event_function"></param>
	/// <param name="evente_begin_time">この時間以降で一度イベントを実行する</param>
	AnimationEventOncePerLoop(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float evente_begin_time);

	/// <summary>
	/// ループ毎に行う初期化処理
	/// </summary>
	void InitializePerLoop();

	/// <summary>
	/// イベントが実行可能か
	/// </summary>
	bool CanExecuteEvent(float time) const;

	/// <summary>
	/// イベントの実行
	/// </summary>
	void Execute(const std::shared_ptr<Actor>& actor);

private:

	bool has_executed_once_in_this_loop;
	float evente_begin_time;
};

/// <summary>
/// 指定時間範囲内で毎フレーム実行されるイベント
/// </summary>
class AnimationEventPerFrame : public AnimationCallbackEvent
{
public:
	/// <summary></summary>
	/// <param name="event_function"></param>
	/// <param name="event_begin_time">イベント実行可能範囲</param>
	/// <param name="event_end_time">イベント実行可能範囲</param>
	AnimationEventPerFrame(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float event_begin_time,
		float event_end_time);

	/// <summary>
	/// イベントが実行可能か
	/// </summary>
	bool CanExecuteEvent(float time) const;

private:

	std::pair<float, float> event_execute_time_range;
};
