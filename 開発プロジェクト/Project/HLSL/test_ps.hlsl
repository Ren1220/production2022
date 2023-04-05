#include "test.hlsli"

Texture2D color_map : register(t0);
SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = color_map.Sample(set_sampler_state, input.texcoord);

    return color * input.color;
}