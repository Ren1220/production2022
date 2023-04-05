#pragma once

/* --- ヘッダファイルのインクルード ---- */

#include <array>

#include "Arithmetic/Arithmetic.h"

/* --- 宣言 ---- */

class Frustum
{
public:

	void Update(const Matrix& view, const Matrix& proj);

	bool IntersectAABB(const Vector3& min, const Vector3& max);

	void DrawDebugPrimitive(const Vector4& color = {1,1,0,1});

private:

	struct Plane
	{
		Vector3 normal; // 法線
		float distance; // 原点からの距離
	};

	struct Vertices
	{
		union
		{
			struct
			{
				Vector3 near_z[4];
				Vector3 far_z[4];
			};

			// near0,1,2,3 far0,1,2,3
			Vector3 pos[8];
		};
	};

	struct AABB
	{
		AABB(const Vector3& min, const Vector3& max)
		{
			CalcParam(min, max);
		}

		void CalcParam(const Vector3& min, const Vector3& max);

		Vector3 center; 
		Vector3 radius; 
	};

	std::array<Plane, 6> frustum; 
	Vertices vertices;
	int32_t collision_state;
};
