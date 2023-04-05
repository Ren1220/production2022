#include "Sprite.hlsli"

Texture2D scene_texture : register(t0);
Texture2D bloom_texture : register(t1);

SamplerState linear_wrap_sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 base_color = scene_texture.Sample(linear_wrap_sampler, input.texcoord);

    float3 bloom = bloom_texture.Sample(linear_wrap_sampler, input.texcoord);
    
    return base_color + float4(bloom, 1.0f);

}