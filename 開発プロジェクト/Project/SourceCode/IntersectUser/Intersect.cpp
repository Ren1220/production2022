#include "Intersect.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Debug/DebugPrimitive.h"

/* --- 定義 ---- */

bool IntersectUser::ClosestPositionPointTriangle(
	const Vector3& point,
	const Vector3 triangle_position[3],
	Vector3& near_position,
	Vector3& near_triangle_position0,
	Vector3& near_triangle_position1)
{
	const Vector3 vec01{ triangle_position[1] - triangle_position[0] };
	const Vector3 vec02{ triangle_position[2] - triangle_position[0] };

	// pointがtriangle_position[0]の外側の頂点領域に存在するか
	const Vector3 vec0P{ point - triangle_position[0] };
	float d1{ vec01.Dot(vec0P) };
	float d2{ vec02.Dot(vec0P) };

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// 最近点はtriangle_position[0]、交差はしていない
		near_position = triangle_position[0];
		near_triangle_position0 = triangle_position[0];
		return false;
	}

	// pointがtriangle_position[1]の外側の頂点領域にあるかチェック
	const Vector3 vec1P{ point - triangle_position[1] };
	float d3 { vec01.Dot(vec1P) };
	float d4 { vec02.Dot(vec1P) };

	if (d3 >= 0.0f && d4 <= d3)
	{
		// 最近点はtriangle_position[1]、交差はしていない
		near_position = triangle_position[1];
		near_triangle_position0 = triangle_position[1];
		return false;
	}

	// pointがtriangle_position[0]とtriangle_position[1]の外側の辺領域にあるかチェック
	float scalar3_01 = d1 * d4 - d3 * d2;

	if (scalar3_01 <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		// 最近点はtriangle_position[0], triangle_position[1]、交差はしていない
		float t = d1 / (d1 - d3);
		near_position = triangle_position[0] + vec01 * t;
		near_triangle_position0 = triangle_position[0];
		near_triangle_position1 = triangle_position[1];
		return false;
	}

	// pointがtriangle_position[2]の外側の頂点領域にあるかチェック
	const Vector3 vec2P{ point - triangle_position[2] };
	float d5 { vec01.Dot(vec2P) };
	float d6 { vec02.Dot(vec2P) };

	if (d6 >= 0.0f && d5 <= d6)
	{
		near_position = triangle_position[2];
		near_triangle_position0 = triangle_position[2];
		return false;
	}

	// pointがtriangle_position[0]とtriangle_position[2]の外側の辺領域にあるかチェック
	float scalar3_02 = d5 * d2 - d1 * d6;

	if (scalar3_02 <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float t = d2 / (d2 - d6);
		near_position = triangle_position[0] + vec02 * t;
		near_triangle_position0 = triangle_position[0];
		near_triangle_position1 = triangle_position[2];
		return false;
	}

	// pointがtriangle_position[1]とtriangle_position[2]の外側の辺領域にあるかチェック
	float scalar3_12 = d3 * d6 - d5 * d4;

	if (scalar3_12 <= 0.0f && d4 >= d3 && d5 >= d6)
	{
		float t = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		near_position = triangle_position[1] + (triangle_position[2] - triangle_position[1]) * t;
		near_triangle_position0 = triangle_position[1];
		near_triangle_position1 = triangle_position[2];
		return false;
	}

	// ここまでくれば、nearPosは三角形の内部にある
	float denom = 1.0f / (scalar3_01 + scalar3_02 + scalar3_12);
	float t01 = scalar3_02 * denom;
	float t02 = scalar3_01 * denom;
	near_position = triangle_position[0] + vec01 * t01 + vec02 * t02;
	return true;
}

bool IntersectUser::ClosestPositionPointTriangle(
	const Vector3& point, 
	const Vector3 triangle_position[3], 
	Vector3& near_poisiton)
{
	Vector3 dummy1;
	Vector3 dummy2;

	return IntersectUser::ClosestPositionPointTriangle(point, triangle_position, near_poisiton, dummy1, dummy2);
}

bool IntersectUser::IntersectSphereVSTriangle(
	const Vector3& sphere_position, 
	const float radius, 
	const Vector3 triangle_position[3],
	Vector3& near_position)
{
	// 球の中心点　から　最も近い頂点　を求める
	ClosestPositionPointTriangle(sphere_position, triangle_position, near_position);
	// 球の中心点と求めた最近点との距離の平方　と　半径の二乗　を比較し、
	// 交差しているか判定する。
	return ((sphere_position - near_position).LengthSquare() <= (radius * radius));
}

bool IntersectUser::IntersectRayVSTriangle(
	const Vector3& ray_start, 
	const Vector3& ray_direction, 
	const float ray_length, 
	const Vector3 triangle_position[3],
	HitResult::ResultRayVSTriangle& result)
{
	const Vector3 ab{ triangle_position[1] - triangle_position[0] };
	const Vector3 ac{ triangle_position[2] - triangle_position[0] };
	const Vector3 cross1{ ab.Cross(ac) };
	// レイの始点　から　終点　へのベクトル
	const Vector3 qp{ ray_start - (ray_start + ray_direction * ray_length) };

	const float dot1{ cross1.Dot(qp) };

	// 表面から交差　しているか
	if (dot1 > 0.0f)
	{
		// レイ　と　法線　が水平でないか。水平の時、レイ　と　三角形　は平行。
		if (fabsf(dot1) > FLT_EPSILON)
		{
			Vector3 ap{ ray_start - triangle_position[0] };
			float dot2{ cross1.Dot(ap) };

			// レイの向き　と　レイの長さ　
			if (dot2 >= 0.0f && dot2 < dot1)
			{
				// 始点　から　頂点　へのベクトルと　始点　から　終点　へのベクトルの外積
				Vector3 cross2{ qp.Cross(ap) }; 
				float dot3{ ac.Dot(cross2) };

				if (dot3 >= 0.0f && dot3 <= dot1)
				{
					float dot4{ -ab.Dot(cross2) };
					if (dot4 > 0.0f && dot3 + dot4 <= dot1)
					{
						result.intersect_position = ray_start + ray_direction * (ray_length * dot2 / dot1);
						result.distance = (ray_length * dot2 / dot1);
						result.triangle_normal = cross1;
						result.triangle_vertices[0] = triangle_position[0];
						result.triangle_vertices[1] = triangle_position[1];
						result.triangle_vertices[2] = triangle_position[2];
						return true;
					}
				}
			}
		}
	}

	result = {};
	return false;
}

bool IntersectUser::IntersectRayVSSphere(
	const Vector3& start, 
	const Vector3& end, 
	const Vector3& sphere_position, 
	const float radius, 
	HitResult::ResultRayVSSphere& result)
{
	Vector3 ray_dir{ end - start };
	Vector3 ray_dir_norm{ ray_dir.Normalize() };
	Vector3 vec0{ sphere_position - start };
	// 始点　から　球の中心点へのベクトル　を　レイのベクトル　に射影する。
	float proj{ vec0.Dot(ray_dir_norm) }; 
	float dist_sq{ vec0.LengthSquare() - proj };

	// 球の半径　が　始点と球の中心点の距離　より大きいか
	if (dist_sq < radius * radius)
	{
		// 比較した値が平方　なので平方根を行う。
		result.distance = proj - sqrtf(radius * radius - dist_sq);
		// 距離　が　正の値　かつ　レイの長さ　より短いか
		if (result.distance > 0.0f && result.distance < ray_dir.Length())
		{
			result.intersect_position = start + ray_dir_norm * result.distance;
			return true;
		}
	}

	result.intersect_position = {};
	result.distance = 0.0f;

	return false;
}

bool IntersectUser::IntersectSphereVSCapsule(
	const Vector3& s_pos, 
	const float s_rad, 
	const Vector3& c_start, 
	const Vector3& c_end, 
	const float c_rad, 
	float& dist)
{
	Vector3 c_vec{ c_end - c_start };
	Vector3 c_dir{ c_vec.Normalize() };

	Vector3 vec{ s_pos - c_start };

	float dot{ vec.Dot(c_dir) };

	Vector3 c_near{};

	// 射影長がマイナスもしくは、円柱の長さを超える場合補正をかける必要がある
	if (dot < 0.0f)
	{
		c_near = c_start;
	}
	else if (dot * dot > c_vec.LengthSquare())
	{
		c_near = c_end;
	}
	// 問題ない場合は射影した位置が最近点
	else
	{
		c_near = c_start + dot * c_dir;
	}

	dist = (s_pos - c_near).Length();

	if (c_rad + s_rad > dist)
	{
		return true;
	}

	return false;
}

bool IntersectUser::IntersectCapsuleVSCapsule(
	const Vector3& a_start, 
	const Vector3& a_end, 
	const float a_rad, 
	const Vector3& b_start, 
	const Vector3& b_end, 
	const float b_rad)
{
	Vector3 dummy;
	return IntersectCapsuleVSCapsule(a_start, a_end, a_rad, b_start, b_end, b_rad, dummy, dummy);
}

bool IntersectUser::IntersectCapsuleVSCapsule(
	const Vector3& a_start, 
	const Vector3& a_end, 
	const float a_rad, 
	const Vector3& b_start, 
	const Vector3& b_end, 
	const float b_rad, 
	Vector3& out_a_near, 
	Vector3& out_b_near)
{
	Vector3 a_vec{ a_end - a_start };
	Vector3 b_vec{ b_end - b_start };

	Vector3 ab_vec{ a_start - b_start };

	float a{ a_vec.Dot(a_vec) };
	float b{ a_vec.Dot(b_vec) };
	float c{ a_vec.Dot(ab_vec) };
	float e{ b_vec.Dot(b_vec) };
	float f{ b_vec.Dot(ab_vec) };

	float denom{ a * e - b * b };

	float a_t{}, b_t{};

	bool is_point[2]{ a < FLT_EPSILON, e < FLT_EPSILON };

	// 両方点
	if (is_point[0] && is_point[1])
	{
		a_t = 0;
		b_t = 0;
	}
	// aのみ点
	else if (is_point[0])
	{
		a_t = 0;
		b_t = f / e;
	}
	// bのみ点
	else if (is_point[1])
	{
		a_t = -c / a;
		b_t = 0;
	}
	else
	{
		a_t = (b * f - c * e) / denom;
		b_t = (a * f - b * c) / denom;
	}

	// 分母がゼロの場合二つの線分が平行である
	if (denom < FLT_EPSILON && denom > -FLT_EPSILON)
	{
		a_t = 0;
	}

	// 算出した距離tを線分内に収める
	a_t = std::clamp(a_t, 0.0f, 1.0f);

	// 線分bに最短点aから射影して最短点bを求める 
	b_t = (a_t * b + f) / e;

	// 線分bの外だった場合、線分bの端を最短点を算出し、線分a上の最短点を再計算
	if (b_t > 1.0f)
	{
		b_t = 1.0f;
		a_t = (b - c) / a;
	}
	else if (b_t < 0.0f)
	{
		b_t = 0.0f;
		a_t = -c / a;
	}

	// 算出した距離tを線分内に収める
	a_t = std::clamp(a_t, 0.0f, 1.0f);

	// 二つの線分の最短点からの距離と二つのカプセルの半径を比較する
	out_a_near = a_start + a_vec * a_t;
	out_b_near = b_start + b_vec * b_t;

	if ((out_b_near - out_a_near).LengthSquare() <= (a_rad + b_rad) * (a_rad + b_rad))
	{
		return true;
	}

	return false;
}

