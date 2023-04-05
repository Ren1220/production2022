#include "Sprite.hlsli"

Texture2D color_texture          : register(t0);
SamplerState color_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    return color_texture.Sample(color_sampler_state, input.texcoord) * material_color * input.color;
}