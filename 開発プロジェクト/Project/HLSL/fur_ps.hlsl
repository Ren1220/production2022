#include "fur.hlsli"

SamplerState linear_wrap : register(s0);
SamplerState linear_border : register(s1);

Texture2D fur_map : register(t0);
Texture2D normal_map : register(t1);

float4 main(PSInput input) : SV_TARGET
{
    // テクセルカラーの取得
    float4 color = fur_map.Sample(linear_wrap, input.texcoord);

    float alpha = (color.a + input.color.a) / 2.0f;

    // ワールド空間における法線
    //float3 N = normalize(input.world_normal.xyz);
    float3 N = 2.0f * normal_map.Sample(linear_wrap, input.texcoord).rgb - 1.0f;

    // 光の逆ベクトル
    float3 L = normalize(light_direction.xyz);

    // 拡散反射
    float3 diffuse = (dot(L, N)) * color.rgb * roughness_factor;

    return float4(Ambient(diffuse), 1.0) * float4(input.color.rgb, alpha);
}
