#include "BlurComposition.hlsli"

Texture2D blur_texture_0 : register(t0);
Texture2D blur_texture_1 : register(t1);
Texture2D blur_texture_2 : register(t2);
Texture2D blur_texture_3 : register(t3);

SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    // ボケ画像をサンプリングして、平均を取って出力する
    float4 color = blur_texture_0.Sample(set_sampler_state, input.texcoord);
    color += blur_texture_1.Sample(set_sampler_state, input.texcoord);
    color += blur_texture_2.Sample(set_sampler_state, input.texcoord);
    color += blur_texture_3.Sample(set_sampler_state, input.texcoord);

    color /= 4.0f;
    color.a = 1.0f;

    return color;
}