#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <memory>
#include <vector>
#include <string>

#include "Graphics/Model/GLTFModelData.h"

#include "AnimationEvent.h"

/* --- 前方宣言 ---- */

class Actor;
class GLTFSkinnedMesh;

/* --- 宣言 ---- */

class Animation
{
public:

	Animation(size_t animation_index);

	/// <summary>
	/// <para>モデルクラスのアニメーションノードにアニメーション結果を適用する。</para>
	/// </summary>
	/// <param name="model"></param>
	/// <param name="time">アニメーションの時間。アニメーションが終了したらその終了時間を設定する</param>
	/// <param name="has_played">アニメーションが終了したらtrueを設定する</param>
	void AnimateModelNode(GLTFSkinnedMesh* model, float& time, bool& has_played);

	/// <summary>
	/// <para>モデルクラスのアニメーションノードにアニメーション結果を適用する。</para>
	/// <para>ループしたとき各アニメーションイベントのループ毎初期化処理が実行される。</para>
	/// </summary>
	/// <param name="model"></param>
	/// <param name="time">アニメーションの時間。アニメーションがループしたら、終了時間で割った余りの時間を設定する</param>
	/// <param name="has_looped">アニメーションがループしたらtrueを設定する</param>
	void AnimateModelNodeLoop(GLTFSkinnedMesh* model, float& time, bool& has_looped);

	/// <summary>
	/// 指定時間範囲内で毎フレーム実行されるアニメーションイベントを登録する
	/// </summary>
	/// <param name="event_function">実行するイベント</param>
	/// <param name="event_begin_time">イベント開始時間</param>
	/// <param name="event_end_time">イベント終了時間</param>
	void AddEventPerFrame(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float event_begin_time,
		float event_end_time);

	/// <summary>
	/// ループ毎に指定時間以降一度だけ実行されるイベント
	/// </summary>
	/// <param name="event_function">実行するイベント</param>
	/// <param name="evente_begin_time">この時間以降で一度イベントを実行する</param>
	void AddEventOncePerLoop(
		const std::function<void(const std::shared_ptr<Actor>&)>& event_function,
		float evente_begin_time);

	/// <summary>
	/// このアニメションを変更した際に実行されるイベント
	/// </summary>
	void SetEventWhenExit(
		const std::function<void(const std::shared_ptr<Actor>&)>& exit_event_function);

	/// <summary>
	/// このアニメションに変更した際に実行されるイベント
	/// </summary>
	void SetEventWhenEnter(
		const std::function<void(const std::shared_ptr<Actor>&)>& enter_event_function);

	/// <summary>
	/// アニメーションの終了時間を計測する
	/// </summary>
	float MeasureAnimationTimeEnd(const GLTFSkinnedMesh* model) const;

	/// <summary>
	/// 現在実行可能なアニメーションイベントを実行する
	/// </summary>
	void ExecuteActiveEvent(const std::shared_ptr<Actor>& actor, float current_animation_time);

	/// <summary>
	/// 登録されている場合、アニメーションの開始イベントを実行する
	/// </summary>
	void ExecuteEventWhenEnter(const std::shared_ptr<Actor>& actor);

	/// <summary>
	/// 登録されている場合、アニメーションの終了イベントを実行する
	/// </summary>
	void ExecuteEventWhenExit(const std::shared_ptr<Actor>& actor);

	void InitializeAnimationEventsPerLoop();

	const size_t& GetAnimationIndex() { return animation_index; }

private:
	
	size_t animation_index;
	std::vector<std::unique_ptr<AnimationCallbackEvent>> animation_events;
	// このアニメションに変更する際に実行されるイベント
	std::unique_ptr<AnimationCallbackEvent> animation_event_when_enter;	
	// このアニメションを変更する際に実行されるイベント
	std::unique_ptr<AnimationCallbackEvent> animation_event_when_exit;  
};
