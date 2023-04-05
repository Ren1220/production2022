#include "IntersectModel.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFModel.h"
#include "Debug/DebugPrimitive.h"

/* --- 定義 ---- */

bool IntersectUser::IntersectRayCastVSModel(
    const Vector3& start, 
    const Vector3& end, 
    const GLTFModel* model, 
    const Matrix& model_world_transform,
    HitResult::ResultRayCastVSModel& result)
{
    const auto& data{ model->GetModelData() };

    for (const auto& node : data->nodes)
    {
        if (node->mesh < 0) continue; // メッシュの存在しないノード

        const auto& mesh{ data->meshes[node->mesh] };

        for (const auto& primitive : mesh.primitives)
        {
            for (size_t i = 0; i < primitive.indices.size(); i += 3ULL)
            {
                // TODO : GLTFは右手系データだからか頂点の順番が逆になっている。
                // 無理矢理正しているだけなので、不具合が出たら直す。
                const Vector3 triangle_position[3]
                {
                    primitive.positions[primitive.indices[i + 0ULL]].Transform(node->global_transform * model_world_transform),
                    primitive.positions[primitive.indices[i + 1ULL]].Transform(node->global_transform * model_world_transform),
                    primitive.positions[primitive.indices[i + 2ULL]].Transform(node->global_transform * model_world_transform),
                };

                HitResult::ResultRayVSTriangle result0{};

                Vector3 vec{ end - start };

                if (IntersectRayVSTriangle(
                    start, 
                    vec.Normalize(), 
                    vec.Length(), 
                    triangle_position, 
                    result0))
                {
                    result.intersect_position = result0.intersect_position;
                    result.distance = result0.distance;
                    result.triangle_normal = result0.triangle_normal;
                    result.triangle_vertices[0] = result0.triangle_vertices[0];
                    result.triangle_vertices[1] = result0.triangle_vertices[1];
                    result.triangle_vertices[2] = result0.triangle_vertices[2];

                    return true;
                }
            }
        }
    }

    return false;
}

bool IntersectUser::IntersectSphereCastVSModel(
    const Vector3& start, 
    const Vector3& end, 
    const float radius, 
    const GLTFModel* model, 
    HitResult::ResultSphereCastVSModel& result)
{
   return IntersectSphereCastVSModel(start, end, radius, model, model->GetWorldTransform(), result);
}

bool IntersectUser::IntersectSphereCastVSModel(
    const Vector3& start, 
    const Vector3& end, 
    const float radius, 
    const GLTFModel* model, 
    const Matrix& model_world_transform, 
    HitResult::ResultSphereCastVSModel& result)
{
    const auto& data{ model->GetModelData() };

    for (const auto& node : data->nodes)
    {
        if (node->mesh < 0) continue; // メッシュの存在しないノード

        const auto& mesh{ data->meshes[node->mesh] };

        for (const auto& primitive : mesh.primitives)
        {
            for (size_t i = 0; i < primitive.indices.size(); i += 3ULL)
            {
                const Vector3 triangle_position[3]
                {
                    primitive.positions[primitive.indices[i + 0ULL]].Transform(node->global_transform * model_world_transform),
                    primitive.positions[primitive.indices[i + 1ULL]].Transform(node->global_transform * model_world_transform),
                    primitive.positions[primitive.indices[i + 2ULL]].Transform(node->global_transform * model_world_transform),
                };

                HitResult::ResultRayVSTriangle result0{};

                if (IntersectSphereCastVSTriangleEST(
                    start,
                    end,
                    radius,
                    triangle_position,
                    result0))
                {
                    result.intersect_position = result0.intersect_position;
                    result.distance = result0.distance;
                    result.triangle_normal = result0.triangle_normal;
                    result.triangle_vertices[0] = result0.triangle_vertices[0];
                    result.triangle_vertices[1] = result0.triangle_vertices[1];
                    result.triangle_vertices[2] = result0.triangle_vertices[2];

                    return true;
                }
            }
        }
    }
    return false;
}
