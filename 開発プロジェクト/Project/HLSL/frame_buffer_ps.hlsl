#include "frame_buffer.hlsli"

Texture2D color_texture : register(t0);
SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = color_texture.Sample(set_sampler_state, input.texcoord);

    // コントラスト補正
    color.rgb = 1.0 / (1.0 + exp(-contrast * (color.rgb - 0.5)));

    return color;
}