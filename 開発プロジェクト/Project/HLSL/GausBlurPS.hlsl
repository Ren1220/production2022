#include "GausBlur.hlsli"

Texture2D scene_texture : register(t0);
SamplerState set_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color;

    // 基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    color = blur_weight[0].x * scene_texture.Sample(set_sampler_state, input.texcoord0.xy);
    color += blur_weight[0].y * scene_texture.Sample(set_sampler_state, input.texcoord1.xy);
    color += blur_weight[0].z * scene_texture.Sample(set_sampler_state, input.texcoord2.xy);
    color += blur_weight[0].w * scene_texture.Sample(set_sampler_state, input.texcoord3.xy);
    color += blur_weight[1].x * scene_texture.Sample(set_sampler_state, input.texcoord4.xy);
    color += blur_weight[1].y * scene_texture.Sample(set_sampler_state, input.texcoord5.xy);
    color += blur_weight[1].z * scene_texture.Sample(set_sampler_state, input.texcoord6.xy);
    color += blur_weight[1].w * scene_texture.Sample(set_sampler_state, input.texcoord7.xy);

    // 基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    color += blur_weight[0].x * scene_texture.Sample(set_sampler_state, input.texcoord0.zw);
    color += blur_weight[0].y * scene_texture.Sample(set_sampler_state, input.texcoord1.zw);
    color += blur_weight[0].z * scene_texture.Sample(set_sampler_state, input.texcoord2.zw);
    color += blur_weight[0].w * scene_texture.Sample(set_sampler_state, input.texcoord3.zw);
    color += blur_weight[1].x * scene_texture.Sample(set_sampler_state, input.texcoord4.zw);
    color += blur_weight[1].y * scene_texture.Sample(set_sampler_state, input.texcoord5.zw);
    color += blur_weight[1].z * scene_texture.Sample(set_sampler_state, input.texcoord6.zw);
    color += blur_weight[1].w * scene_texture.Sample(set_sampler_state, input.texcoord7.zw);
    
    return color;
    
}