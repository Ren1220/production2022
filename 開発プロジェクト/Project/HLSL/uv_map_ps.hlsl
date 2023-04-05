#include "uv_map.hlsli"

Texture2D color_map : register(t0);
SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    return color_map.Sample(set_sampler_state, input.texcoord);
}