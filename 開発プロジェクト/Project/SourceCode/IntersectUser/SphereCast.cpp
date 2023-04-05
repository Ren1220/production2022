#include "SphereCast.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFModelData.h"
#include "Debug/DebugPrimitive.h"
#include "IntersectUser/Intersect.h"


/* --- 定義 ---- */

bool IntersectUser::IntersectSphereCastVSTriangleEST(
	const Vector3& start,
	const Vector3& end,
	const float radius,
	const Vector3 triangle_position[3],
	HitResult::ResultRayVSTriangle& result)
{
	// 三角形　と　始点における球　が交差している場合は、false を返す。。
	if (IntersectSphereVSTriangle(start, radius, triangle_position, result.intersect_position))
	{
		return false;
	}

	Vector3 ray_dir{ end - start };
	Vector3 ab{ triangle_position[1] - triangle_position[0] };
	Vector3 ac{ triangle_position[2] - triangle_position[0] };
	Vector3 cross1{ ab.Cross(ac) }; // 三角形の法線

	Vector3 vec1{ start - end }; // 終点から始点へのベクトル

	// 三角形の法線　と　終点ー始点ベクトル　の　内積値
	float dot1{ cross1.Dot(vec1) };

	// 表面から交差していなければ false を返す。
	if (dot1 <= 0.0f)
	{
		return false;
	}

	// レイ　と　三角形の法線　が　水平な場合、レイ　と　三角形は平行なので、false を返す。
	if (fabsf(dot1) <= FLT_EPSILON) // 限りなく０に近い場合
	{
		return false;
	}

	// レイの始点　を　三角形の法線　を　基に球表面の最短点　に移動させる。
	// https://www.geogebra.org/calculator/nsegxbkk
	// 三角形の正規化済み逆法線　から　球の半径分　ずらしたベクトル。
	Vector3 fix_vec{ (-cross1).Normalize() * radius };
	Vector3 start_fix{ start + fix_vec }; // 補正後の始点

	// 頂点０　と　補正後の始点　のベクトル
	Vector3 vec3{ start_fix - triangle_position[0] };
	float dot2 = cross1.Dot(vec3);

	// 三角形の法線　と　補正後のレイ　が向かい合っているか、
	// レイの長さは
	if (dot2 >= 0.0f && dot2 < dot1)
	{
		// レイ　と　三角形を含む平面の交点　の算出

		Vector3 cross2{ vec1.Cross(vec3) };

		float dot3{ ac.Dot(cross2) };
		float dot4{ -ab.Dot(cross2) };
		dot2 /= dot1;
		result.intersect_position = start_fix + ray_dir * dot2;

		if (dot3 >= 0.0f && dot3 <= dot1 && dot4 > 0.0f && dot3 + dot4 <= dot1)
		{
			// レイ　と　三角形を含む平面の交点　が三角形の内部
			result.distance = ray_dir.Length() * dot2;
			result.triangle_normal = cross1;
			result.triangle_vertices[0] = triangle_position[0];
			result.triangle_vertices[1] = triangle_position[1];
			result.triangle_vertices[2] = triangle_position[2];
			return true;
		}
		else
		{
			// レイ　と　三角形を含む平面の交点　が三角形の外部

			// 交点に最も近い三角形内部の点　を求める
			Vector3 near_position{};
			ClosestPositionPointTriangle(result.intersect_position, triangle_position, near_position);

			HitResult::ResultRayVSSphere result0{  };
			// 交点に一番近い三角形内部の点から、逆向きのレイを発射し、
			// レイの始点にある球体と交差するか確認
			// 交差した場合、
			// 交点に一番近い三角形内部の点がスフィアキャストの三角形との交点になる
			if (IntersectRayVSSphere(
				near_position,
				near_position - ray_dir,
				start,
				radius,
				result0))
			{
				result.intersect_position = near_position;
				result.distance = result0.distance;
				result.triangle_normal = cross1; 
				result.triangle_vertices[0] = triangle_position[0];
				result.triangle_vertices[1] = triangle_position[1];
				result.triangle_vertices[2] = triangle_position[2];
				return true;
			}
		}
	}

	return false;
}
