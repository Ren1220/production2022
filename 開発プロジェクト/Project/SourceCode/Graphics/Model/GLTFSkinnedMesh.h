#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include "GLTFModel.h"

#include "../Resource/ConstantBuffer.h"

/* --- 宣言 ---- */


class GLTFSkinnedMesh : public GLTFModel
{
public:

	GLTFSkinnedMesh(ID3D11Device* device, const char* filepath);
	virtual ~GLTFSkinnedMesh();

	/// <param name="shader">シェーダを設定しない場合は汎用シェーダーを利用する</param>
	void Render(ID3D11DeviceContext* dc, Shader* shader = nullptr) const override;

	/// <summary>
	/// ノードにアニメション結果を適用する。
	/// モデルデータに設定されているアニメーションを参照する。
	/// </summary>
	/// <param name="animation_index">参照するモデルデータのアニメション番号</param>
	/// <param name="time">経過時間</param>
	/// <param name="loop">ループさせるか</param>
	/// <returns>アニメーションが再生終了しているか、アニメーションが存在していない場合もfalseを返す</returns>
	bool Animate(size_t animation_index, float time, bool loop);

	/// <summary>
	/// ノードにアニメション結果を適用する。
	/// 外部アニメーションを参照する。
	/// </summary>
	/// <param name="animation"></param>
	/// <param name="time">アニメーションの時間。アニメーションが終了したらその終了時間を設定する</param>
	/// <param name="has_played">アニメーションが終了したらtrueを設定する</param>
	void Animate(const GLTFModelData::Animation& animation, float& time, bool& has_played);

	bool Animate(size_t animation_index, float& time);
	void AnimateLoop(size_t animation_index, float& time, bool& has_looped);

	/// <summary>
	/// ノードにアニメション結果を適用する
	/// 外部アニメーションを参照する。
	/// </summary>
	/// <param name="animation"></param>
	/// <param name="time">アニメーションの時間。アニメーションがループしたら、終了時間で割った余りの時間を設定する</param>
	/// <param name="has_looped">アニメーションがループしたらtrueを設定する</param>
	void AnimateLoop(const GLTFModelData::Animation& animation, float& time, bool& has_looped);

	void BlendAnimation(size_t index1, size_t index2, float time1, float time2, float interpolation);

	float GetAnimationLength(size_t index) const;

	/// <summary>
	/// アニメションノードの取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::shared_ptr<GLTFModelData::Node>>& GetAnimatedNodes() const;

	/// <summary>
	/// 引数の名前のアニメションノードのワールド座標を返す(当たり判定の奴用にワールド平行移動が計算に含まれていない)
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	Vector3 GetAnimatedNodeWorldSR(const char* name) const;

	std::shared_ptr<GLTFModelData::Node> FindAnimatedNode(const char* node_name);

private:

	/// <summary>
	/// ノードインデックスのメッシュを描画する再帰関数
	/// </summary>
	void DrawMeshRecursive(ID3D11DeviceContext* dc, int32_t node_index) const;
	
	// アニメーションを行い、頂点シェーダーにてアニメーション結果を反映させるためのノード配列
	std::vector<std::shared_ptr<GLTFModelData::Node>> animated_nodes; 

protected:

	struct CBufferPrimitive
	{
		static constexpr uint32_t slot{ 0 };

		Matrix world;
		int material{ -1 };
		int has_tangent{ 0 }; // こっちはint型
		int skin{ -1 };
		int pad;
	};

	static constexpr size_t PrimitiveMaxJoints{ 512 };

	/// <summary>
	/// ボーン行列定数バッファ
	/// </summary>
	struct CBufferJoint
	{
		static constexpr uint32_t slot{ 2 };
		Matrix matrices[PrimitiveMaxJoints];
	};

	ConstantBuffer<CBufferPrimitive> primitive_cb; // プリミティブ定数バッファ
	ConstantBuffer<CBufferJoint>	 joint_cb; // ボーン行列定数バッファ
};
