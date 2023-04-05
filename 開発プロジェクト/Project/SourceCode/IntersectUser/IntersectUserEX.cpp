#include "IntersectUserEX.h"

/* --- 追加のヘッダファイルのインクルード ---- */

#include "Graphics/Model/GLTFModel.h"

/* --- 定義 ---- */

bool IntersectUser::IntersectSphereCastVSModelEX(
    const Vector3& start,
    const Vector3& end,
    const float radius,
    const GLTFModel* model,
    const Matrix& model_world_transform,
    const TileData::CollideFlag flag,
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

                Vector3 normal{ primitive.normals[primitive.indices[i + 0ULL]] };

                bool can_collide{ false };


                // 交差判定を行う面か
                if (normal.x > 0.0f)
                {
                    if ((flag & TileData::C_CollideRight) != 0)
                    {
                        continue;
                    }
                }
                else if (normal.x < 0.0f)
                {
                    if ((flag & TileData::C_CollideLeft) != 0)
                    {
                        continue;
                    }
                }
                else if (normal.z > 0.0f)
                {
                    if ((flag & TileData::C_CollideUp) != 0)
                    {
                        continue;
                    }
                }
                else if (normal.z < 0.0f)
                {
                    if ((flag & TileData::C_CollideDown) != 0)
                    {
                        continue;
                    }
                }

                if (!can_collide) continue;

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
