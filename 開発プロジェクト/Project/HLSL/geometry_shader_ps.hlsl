#include "geometry_shader.hlsli"

Texture2D color_map : register(t0);
Texture2D dissolve_map : register(t1);
SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = input.color;

    if (color_texture > 0)
    {
        color *= color_map.Sample(set_sampler_state, input.texcoord);
    }

    if (dissolve_texture > 0)
    {
        float dissolve = 1.0f - dissolve_map.Sample(set_sampler_state, input.texcoord).r;
        dissolve += input.dissolve;
        color.a -= dissolve.r;
    }

    return color;
}