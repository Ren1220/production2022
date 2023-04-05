#include "tessellation.hlsli"

float4 main(DSOutput input) : SV_TARGET
{
    float3 N = float3(0, 1, 0);

    // Œõ‚Ì‹tƒxƒNƒgƒ‹
    float3 L = normalize(light_direction.xyz);

    float3 diffuse = (dot(L, N) * 0.5f + 0.5f) * material_color * roughness_factor;

    return float4(diffuse + float3(0.2f, 0.2f, 0.2f), 1.0);
}