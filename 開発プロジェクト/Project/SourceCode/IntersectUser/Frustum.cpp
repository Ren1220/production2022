#include "Frustum.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Debug/DebugPrimitive.h"

/* --- 定義 ---- */

void Frustum::Update(const Matrix& view, const Matrix& proj)
{
	// ビュープロジェクション
	Matrix view_proj{ view * proj };
	// ビュープロジェクション逆行列
	Matrix ivp{ view_proj.Inverse() };

	// ビュープロジェクション内の頂点算出用位置ベクトル
	constexpr Vector3 vert[8]
	{
		// near
		{ -1.0f, -1.0f, 0.0f },	// 左下
		{  1.0f, -1.0f, 0.0f },	// 右下
		{  1.0f,  1.0f, 0.0f },	// 右上
		{ -1.0f,  1.0f, 0.0f },	// 左上

		// far
		{ -1.0f, -1.0f, 1.0f },	// 左下
		{  1.0f, -1.0f, 1.0f },	// 右下
		{  1.0f,  1.0f, 1.0f },	// 右上
		{ -1.0f,  1.0f, 1.0f }, // 左上
	};

	// ビュープロジェクション行列の逆行列を用いて、各頂点を算出する

	for (int i = 0; i < 8; ++i)
	{
		vertices.pos[i] = vert[i].TransformCoordinate(ivp);
	}

	// 0:左側面, 1:右側面, 2:下側面, 3:上側面, 4:奥側面, 5:手前側面
	// 全ての面の法線は内側を向くように設定する
	// 正規化もする
	frustum[0].normal = Vector3( ivp._14 + ivp._11,  ivp._24 + ivp._21,  ivp._34 + ivp._31).Normalize();
	frustum[1].normal = Vector3( ivp._14 - ivp._11,  ivp._24 - ivp._21,  ivp._34 - ivp._31).Normalize();
	frustum[2].normal = Vector3( ivp._14 + ivp._12,  ivp._24 + ivp._22,  ivp._34 + ivp._32).Normalize();
	frustum[3].normal = Vector3( ivp._14 - ivp._12,  ivp._24 - ivp._22,  ivp._34 - ivp._32).Normalize();
	frustum[4].normal = Vector3(-ivp._14 - ivp._13, -ivp._24 - ivp._23, -ivp._34 - ivp._33).Normalize();
	frustum[5].normal = Vector3(-ivp._14 + ivp._13, -ivp._24 + ivp._23, -ivp._34 + ivp._33).Normalize();

	// 原点からの距離
	frustum[0].distance = vertices.near_z[0].Dot(frustum[0].normal);
	frustum[1].distance = vertices. far_z[2].Dot(frustum[1].normal);
	frustum[2].distance = vertices.near_z[0].Dot(frustum[2].normal);
	frustum[3].distance = vertices. far_z[2].Dot(frustum[3].normal);
	frustum[4].distance = vertices. far_z[2].Dot(frustum[4].normal);
	frustum[5].distance = vertices.near_z[0].Dot(frustum[5].normal);
}

bool Frustum::IntersectAABB(const Vector3& min, const Vector3& max)
{
	constexpr auto calc_min_max = 
		[](float norm_param, 
		   float radius, 
		   float& nega,
		   float& posi)
	{
		if (norm_param >= 0)
		{
			posi += radius;
			nega -= radius;
		}
		else
		{
			posi -= radius;
			nega += radius;
		}
	};

	AABB aabb(min, max);
	collision_state = 0;

	for (int i = 0; i < 6; ++i)
	{
		auto& frus{ frustum[i] };

		// 各平面の法線の成分を用いてAABBの８頂点の中から最近点と最遠点を求める
		Vector3 nega = aabb.center;	// 最近点
		Vector3 posi = aabb.center;	// 最遠点

		// 各成分の最近点、最遠点
		calc_min_max(frus.normal.x, aabb.radius.x, nega.x, posi.x);
		calc_min_max(frus.normal.y, aabb.radius.y, nega.y, posi.y);
		calc_min_max(frus.normal.z, aabb.radius.z, nega.z, posi.z);

		float d_nega{ nega.Dot(frus.normal) };
		float d_posi{ posi.Dot(frus.normal) };

		if (d_posi < frus.distance && d_nega < frus.distance)
		{
			collision_state = 2;
			break;
		}
		else if (d_posi <= frus.distance && d_nega > frus.distance)
		{
			collision_state = 1;
		}
		else if (d_posi > frus.distance && d_nega <= frus.distance)
		{
			collision_state = 1;
		}
	}

	// 視錐台の後ろ側のAABBが現在のアルゴリズムでは条件を見たる可能性がある為、その判定を行う。
	// AABBの後ろ面の中心点と法線を作り、視錐台の８頂点すべてが表側にあれば交差していない。
	if (collision_state = 1)
	{
		Vector3 fface{ aabb.center.x,  aabb.center.y, aabb.center.z - aabb.radius.z };
		Vector3 l_t_f{ aabb.center.x - aabb.radius.x, aabb.center.y + aabb.radius.y, aabb.center.z - aabb.radius.z };
		Vector3 r_t_f{ aabb.center.x + aabb.radius.x, aabb.center.y + aabb.radius.y, aabb.center.z - aabb.radius.z };
		Vector3 r_b_f{ aabb.center.x + aabb.radius.x, aabb.center.y - aabb.radius.y, aabb.center.z - aabb.radius.z };

		// 後ろ面の法線
		Vector3 n{ ((l_t_f - r_t_f).Cross(r_b_f - l_t_f)).Normalize() };

		// 後ろ面の法線と各頂点の内積値から向きを求め、表裏判定を行う。
		for (int i = 0; i < 8; ++i)
		{
			// 面の中心点から各頂点への正規化ベクトル
			Vector3 vecn{ (fface - vertices.pos[i]).Normalize() };

			float d{ n.Dot(vecn) };

			if (d > 0)
			{
				return true;
			}
		}

		return false;
	}
	else if (collision_state == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}

void Frustum::DrawDebugPrimitive(const Vector4& color)
{
	auto& debug{ DebugPrimitive::GetInstance() };

	debug.DrawLine(vertices.near_z[0], vertices.far_z[0], color);
	debug.DrawLine(vertices.near_z[1], vertices.far_z[1], color);
	debug.DrawLine(vertices.near_z[2], vertices.far_z[2], color);
	debug.DrawLine(vertices.near_z[3], vertices.far_z[3], color);
}

void Frustum::AABB::CalcParam(
	const Vector3& min,
	const Vector3& max)
{
	radius = max - min;
	center = (min + max) / 2;
}
