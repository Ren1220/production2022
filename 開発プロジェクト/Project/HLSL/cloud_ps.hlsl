#include "cloud.hlsli"

Texture2D color_map : register(t0);
SamplerState linear_wrap : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = color_map.Sample(linear_wrap, input.texcoord);

    return color;
}