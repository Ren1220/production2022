#include "GLTFSkinnedMesh.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include <functional>

#include "imgui.h"

#include "Include/misc.h"

#include "Graphics/General/Graphics.h"
#include "Graphics/General/DXHelperUsers.h"
#include "Graphics/General/ResourceFileloader.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Resource/ConstantBufferManager.h"

#include "GLTFModelResource.h"

/* --- 定義 ---- */

GLTFSkinnedMesh::GLTFSkinnedMesh(ID3D11Device* device, const char* filepath)
	: GLTFModel(device, filepath)
{

	// アニメションノードの設定
	const auto& nodes{ GetModelData()->nodes };

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		std::shared_ptr<GLTFModelData::Node>& animated_node{ animated_nodes.emplace_back() };
		// 値のコピーをし、新しくメモリを確保する
		animated_node = std::make_shared<GLTFModelData::Node>(*nodes.at(i));
	}

	// プリミティブ定数バッファ
	primitive_cb.Create(device);
	// ボーン行列定数バッファ
	joint_cb.Create(device);
}

GLTFSkinnedMesh::~GLTFSkinnedMesh()
{
}

void GLTFSkinnedMesh::Render(ID3D11DeviceContext* dc, Shader* shader) const
{
	auto model_data{ GetModelData() };

	if (shader)
	{
		shader->Activate(dc);
	}
	else
	{
		// シェーダを設定しない場合は汎用シェーダーを利用する
		model_data->ActivateDefaultShader(dc);
	}

	// 入力レイアウトはすべてのモデル描画シェーダーで共通
	dc->IASetInputLayout(model_data->GetInputLayout().Get());

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルデータを参照するシェーダリソースビュー
	dc->PSSetShaderResources(0, 1, model_data->material_resource_view.GetAddressOf());
	
	for (const auto& node_index : model_data->scenes.at(0).nodes)
	{
		// ノードごとにノードのメッシュを描画する再帰関数
		DrawMeshRecursive(dc, node_index);
	}
}

bool GLTFSkinnedMesh::Animate(size_t animation_index, float time, bool loop)
{
	using namespace std;

	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	// キーフレームの場所と補間値を返す関数
	std::function<size_t(const vector<float>&, float&, float&, bool, bool&)> index_of{
		[](const vector<float>& timelines, float &time, float& interpolation_factor, bool loop, bool& end)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			// アニメーションが終わった時
			if (time > timelines.at(keyframe_count - 1))
			{
				// ループする場合
				if (loop)
				{
					// 現在のキーフレームを全体のキーフレーム数で割った値を新しくキーフレームに設定する
					time = fmodf(time, timelines.at(keyframe_count - 1));
				}
				// ループしない場合
				else
				{
					// アニメーションしたノードの位置がずれないように補間値を最大にし、
					// 最後からひとつ前のキーフレームを返す
					interpolation_factor = 1.0f;
					end = false; // 再生終了フラグ
					return keyframe_count - 2;
				}
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}
			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};

	// 再生フラグ
	bool is_play{ true };

	// アニメション情報が存在する場合
	if (model_data->animations.size() > 0)
	{
		const GLTFModelData::Animation& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する、再生終了しているかどうかも
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor, loop, is_play) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };

				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();

				//XMFLOAT4 q0{ &rotations.at(keyframe_index).x };
				//XMFLOAT4 q1{ &rotations.at(keyframe_index + 1).x };

				//XMFLOAT4 q;
				//XMStoreFloat4(&q, XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), interpolation_factor));

				//animated_nodes.at(channel.target_node).rotation = &q.x;

			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);
	}
	else
	{
		animated_nodes = model_data->nodes;
	}

	return is_play;
}


bool GLTFSkinnedMesh::Animate(size_t animation_index, float& time)
{
	using namespace std;
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&)> index_of =
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t {
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// アニメーションが終わった時
			if (time > end_time)
			{
				// アニメーションしたノードの位置がずれないように補間値を最大にし、
				// 最後からひとつ前のキーフレームを返す
				interpolation_factor = 1.0f;
				return keyframe_count - 2;
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
	};

	if(model_data->animations.size() > 0)
	{
		const GLTFModelData::Animation& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);

		// アニメーションが終了したか
		float end{ animation.MeasureAnimationTimeEnd() };
		if (time > end)
		{
			time = end;
			return true;
		}
	}

	return false;
}

void GLTFSkinnedMesh::AnimateLoop(size_t animation_index, float& time, bool& has_looped)
{
	has_looped = false;

	using namespace std;
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	if (model_data->animations.size() <= animation_index) return;

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// アニメーションが終わった時
			if (time > end_time)
			{
				// 現在のキーフレームを全体のキーフレーム数で割った値を新しくキーフレームに設定する
				time = fmodf(time, end_time);
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};

	if(model_data->animations.size() > 0)
	{
		const auto& animation{ model_data->animations.at(animation_index) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);

		// アニメーションがループしたか
		float end{ animation.MeasureAnimationTimeEnd() };
		if (time > end)
		{
			time = fmodf(time, end);
			has_looped = true;
		}
	}


	return;
}

#if 0
bool GLTFModel::Animate(const GLTFModelData::Animation& animation, float& time, bool loop)
{
	using namespace std;
	// モデルデータの参照の取得
	std::shared_ptr<GLTFModelData> model_data{ GetModelData() };

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&, bool, bool&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor, bool loop, bool& end)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			// アニメーションが終わった時
			if (time > timelines.at(keyframe_count - 1))
			{
				// ループする場合
				if (loop)
				{
					// 現在のキーフレームを全体のキーフレーム数で割った値を新しくキーフレームに設定する
					time = fmodf(time, timelines.at(keyframe_count - 1));
				}
				// ループしない場合
				else
				{
					// アニメーションしたノードの位置がずれないように補間値を最大にし、
					// 最後からひとつ前のキーフレームを返す
					interpolation_factor = 1.0f;
					end = false; // 再生終了フラグ
					return keyframe_count - 2;
				}
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}
			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};

	// 再生フラグ
	bool is_play{ true };

	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する、再生終了しているかどうかも
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor, loop, is_play) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);
	}

	return is_play;
}
#endif

void GLTFSkinnedMesh::AnimateLoop(const GLTFModelData::Animation& animation, float& time, bool& has_looped)
{
	has_looped = false;

	using namespace std;
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// アニメーションが終わった時
			if (time > end_time)
			{
				// 現在のキーフレームを全体のキーフレーム数で割った値を新しくキーフレームに設定する
				time = fmodf(time, end_time);
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};


	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);
	}

	// アニメーションがループしたか
	float end{ animation.MeasureAnimationTimeEnd() };
	if (time > end)
	{
		time = fmodf(time, end);
		has_looped = true;
	}

	return;
}

void GLTFSkinnedMesh::BlendAnimation(size_t index1, size_t index2, float time1, float time2, float interpolation)
{
	using namespace std;
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	if (index1 < 0 || index2 < 0 || index1 >= model_data->animations.size() || index2 >= model_data->animations.size()) return;

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// アニメーションが終わった時
			if (time > end_time)
			{
				// 現在のキーフレームを全体のキーフレーム数で割った値を新しくキーフレームに設定する
				time = fmodf(time, end_time);
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};

	std::vector<GLTFModelData::Node> node1(animated_nodes.size());
	for (size_t i = 0ULL; i < animated_nodes.size(); ++i)
	{
		node1[i] = *animated_nodes[i];
	}
	std::vector<GLTFModelData::Node>  node2 = node1;

	if (model_data->animations.size() > 0)
	{
		
		const auto& animation{ model_data->animations.at(index1) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time1, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				node1.at(channel.target_node).scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				node1.at(channel.target_node).rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				node1.at(channel.target_node).translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}
	}

	if (model_data->animations.size() > 0)
	{
		const auto& animation{ model_data->animations.at(index2) };

		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time2, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				node2.at(channel.target_node).scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				node2.at(channel.target_node).rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				node2.at(channel.target_node).translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}
	}

	for (size_t i = 0ULL; i < animated_nodes.size(); ++i)
	{
		auto& n{ animated_nodes[i] };

		const auto& n1{ node1[i] };
		const auto& n2{ node2[i] };

		n->rotation    = n1.rotation.Slerp(n2.rotation, interpolation);
		n->scale       = n1.scale.Lerp(n2.scale, interpolation);
		n->translation = n1.translation.Lerp(n2.translation, interpolation);
	}

	// グローバル行列を更新する
	model_data->CumulateTransforms(animated_nodes);
}

float GLTFSkinnedMesh::GetAnimationLength(size_t index) const
{
	return GetModelData()->animations[index].MeasureAnimationTimeEnd();
}

void GLTFSkinnedMesh::Animate(const GLTFModelData::Animation& animation, float& time, bool& has_played)
{
	has_played = false;

	using namespace std;
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };

	// キーフレームの場所と補間値を返す関数
	function<size_t(const vector<float>&, float&, float&)> index_of{
		[](const vector<float>& timelines, float& time, float& interpolation_factor)-> size_t
		{
			const size_t keyframe_count{ timelines.size() };

			float end_time { timelines.at(keyframe_count - 1) };

			// アニメーションが終わった時
			if (time > end_time)
			{
				// アニメーションしたノードの位置がずれないように補間値を最大にし、
				// 最後からひとつ前のキーフレームを返す
				interpolation_factor = 1.0f;
				return keyframe_count - 2;
			}
			// アニメーション開始前
			else if (time < timelines.at(0))
			{
				interpolation_factor = 0.0f;
				return 0;
			}
			// アニメション中

			// キーフレームの場所を探す
			size_t keyframe_index{ 0 };
			for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
			{
				// 現在の時間がタイムラインの時間を超えた地点のキーフレームを設定する
				if (time < timelines.at(time_index))
				{
					keyframe_index = max<size_t>(0LL, time_index - 1);
					break;
				}
			}

			// 補間値を計算する
			interpolation_factor = (time - timelines.at(keyframe_index)) /
					(timelines.at(keyframe_index + 1)) - timelines.at(keyframe_index);

			// キーフレーム番号を返す
			return keyframe_index;
		}
	};


	{
		for (vector<GLTFModelData::Animation::Channel>::const_reference channel : animation.channels)
		{
			const GLTFModelData::Animation::Sampler& sampler{ animation.samplers.at(channel.sampler) };
			const vector<float>& timeline{ animation.timelines.at(sampler.input) };

			if (timeline.size() == 0)
			{
				continue;
			}

			float interpolation_factor{ };
			// 現在のキーフレームの状態を取得する
			size_t keyframe_index{ index_of(timeline, time, interpolation_factor) };
			// アニメションの対象がスケールの場合
			if (channel.target_path == "scale")
			{
				const vector<Vector3>& scales{ animation.scales.at(sampler.output) };
				animated_nodes.at(channel.target_node)->scale = scales.at(keyframe_index).Lerp(scales.at(keyframe_index + 1), interpolation_factor);
			}
			else if (channel.target_path == "rotation")
			{
				const vector<Quaternion>& rotations{ animation.rotations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->rotation = rotations.at(keyframe_index).Slerp(rotations.at(keyframe_index + 1), interpolation_factor).Normalize();
			}
			else if (channel.target_path == "translation")
			{
				const vector<Vector3>& translations{ animation.translations.at(sampler.output) };
				animated_nodes.at(channel.target_node)->translation = translations.at(keyframe_index).Lerp(translations.at(keyframe_index + 1), interpolation_factor);
			}
		}

		// グローバル行列を更新する
		model_data->CumulateTransforms(animated_nodes);
	}

	// アニメーションが終了したか
	float end{ animation.MeasureAnimationTimeEnd() };
	if (time > end)
	{
		time = end;
		has_played = true;
	}

	return;
}

void GLTFSkinnedMesh::DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const
{
	// モデルデータの参照の取得
	auto model_data{ GetModelData() };
	// インデックスのノードの参照
	const std::shared_ptr<GLTFModelData::Node>& node{ model_data->nodes.at(node_index) };

	// スキン(アニメション)情報が存在する場合
	if (node->skin > -1)
	{
		const GLTFModelData::Skin& skin{ model_data->skins.at(node->skin) };

		// ボーン行列定数バッファの設定
		{
			CBufferJoint cb{ };

			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
			{
				cb.matrices[joint_index] =
					skin.inverse_bind_matrices.at(joint_index) *
					animated_nodes.at(skin.joints.at(joint_index))->global_transform *
					node->global_transform.Inverse();
			}

			joint_cb.UpdateSubresouce(dc, cb);
			Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferJoint::slot, &joint_cb);
		}
	}

	// メッシュが存在する場合
	if (node->mesh > -1)
	{
		const GLTFModelData::Mesh& mesh{ model_data->meshes.at(node->mesh) };

		// プリミティブ単位での描画
		for (std::vector<GLTFModelData::Mesh::Primitive>::const_reference primitive : mesh.primitives)
		{
			// 頂点バッファの設定
			{
				// 頂点シェーダーがSoAなので、それに合わせた形で頂点バッファを作成する
				ID3D11Buffer* vertex_buffers[]
				{
					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
				};

				UINT strides[]
				{
					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
				};

				UINT offsets[_countof(vertex_buffers)]{ 0 };

				dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
				dc->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
					primitive.index_buffer_view.format, 0);
			}

			// プリミティブ定数バッファの設定
			{
				CBufferPrimitive cb{ };
				const auto& world_transform{ GetWorldTransform() };

				cb.material = primitive.material;
				cb.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != nullptr;
				cb.skin = node->skin;

				cb.world = node->global_transform * world_transform;

				primitive_cb.UpdateSubresouce(dc, cb);
				Graphics::GetInstance().GetConstantBufferManager()->SetBuffer(dc, CBufferPrimitive::slot, &primitive_cb);
			}

			// テクスチャ情報の設定
			{
				if (primitive.material > -1)
				{
					const GLTFModelData::Material& material{ model_data->materials.at(primitive.material) };

					// テクスチャインデックス配列
					const int texture_indices[]
					{
						material.data.pbr_metallic_roughness.basecolor_texture.index,         // カラー
						material.data.pbr_metallic_roughness.metallic_roughness_texture.index,// 粗さと反射率
						material.data.normal_texture.index,									  // 法線
						material.data.emissive_texture.index,								  // 自身からの発光
						material.data.occlusion_texture.index,								  // 陰影
					};

					ID3D11ShaderResourceView* null_shader_resource_view{ };
					// テクスチャインデックス配列の大きさの配列を作成する
					std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));

					for (int32_t texture_index = 0; texture_index < shader_resource_views.size(); ++
						texture_index)
					{
						// プリミティブに番号の種類のテクスチャが設定されている場合は情報を渡し、設定されていない場合は空の情報を渡す。
						shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
							model_data->texture_resource_views.
							at(model_data->textures.
								at(texture_indices[texture_index]).
								source).Get() :
							null_shader_resource_view;
					}

					// ピクセルシェーダーにテクスチャ情報を渡す
					dc->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()),
						shader_resource_views.data());
				}
			}

			// 描画実行
			dc->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.Count()), 0, 0);
		}
	}

	// 子ノードが存在する場合
	for (std::vector<int32_t>::value_type child_index : node->children)
	{
		DrawMeshRecursive(dc, child_index);
	}
}

const std::vector<std::shared_ptr<GLTFModelData::Node>>& GLTFSkinnedMesh::GetAnimatedNodes() const
{
	return animated_nodes;
}

Vector3 GLTFSkinnedMesh::GetAnimatedNodeWorldSR(const char* name) const
{
	using namespace DirectX;

	// 引数の名前のノードを検索する
	const auto itr{ std::ranges::find_if(animated_nodes,
										 [&](const auto& node) { return node->name == name; }) };

	// ノードが見つかった場合そのワールド座標を返す
	if (itr != animated_nodes.end())
	{
		Vector3 out{};
		const auto& world_transform{ GetWorldTransform() };

		Matrix w
		{
			world_transform._11, world_transform._12, world_transform._13, world_transform._14,
			world_transform._21, world_transform._22, world_transform._23, world_transform._24,
			world_transform._31, world_transform._32, world_transform._33, world_transform._34,
			0, 0, 0, 1
		};

		return out.Transform((*itr)->global_transform * w);
	}

	_ASSERT_EXPR(itr != animated_nodes.end(), L"ノードが見つかりませんでした");
	return {};
}

std::shared_ptr<GLTFModelData::Node> GLTFSkinnedMesh::FindAnimatedNode(const char* node_name)
{
	auto find_node_iterator{ std::ranges::find_if(animated_nodes, [&](const auto& node) { return node->name == node_name; }) };

	if(find_node_iterator != animated_nodes.end())
	{
		return (*find_node_iterator);
	}

	return nullptr;
}

