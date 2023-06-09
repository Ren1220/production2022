#include "ShadowCastCubeMap.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    // 視点からの距離二乗を深度として書き込む
    float3 v = input.world_position - cubemap_view_eye;
    float depth = dot(v, v);

    return float4(depth, 0, 0, 1.0f);
}