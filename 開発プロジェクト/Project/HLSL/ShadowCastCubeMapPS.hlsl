#include "ShadowCastCubeMap.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    // ‹“_‚©‚ç‚Ì‹——£“ñæ‚ğ[“x‚Æ‚µ‚Ä‘‚«‚Ş
    float3 v = input.world_position - cubemap_view_eye;
    float depth = dot(v, v);

    return float4(depth, 0, 0, 1.0f);
}