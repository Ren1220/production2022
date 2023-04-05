#include "LuminanceSampling.hlsli"

Texture2D scene_texture : register(t0);
SamplerState scene_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = scene_texture.Sample(scene_sampler_state, input.texcoord);
	
        

    float t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));

    // clip()関数は引数の値がマイナスになると、以降の処理をスキップする
    // なので、マイナスになるとピクセルカラーは出力されない
    // 今回の実装はカラーの明るさが1以下ならピクセルキルする
    clip(t - 1.0f);

    static const float a = 2.51f;
    static const float b = 0.03f;
    static const float c = 2.43f;
    static const float d = 0.59f;
    static const float e = 0.14f;
    
    float3 m = color.rgb * (a * color.rgb + b);
    float3 M = color.rgb * (c * color.rgb + d) + e;
    color.rgb = m / M;
    
    
    //float3 M = float3(1, 1, 1) + color.rgb;
    //color.rgb /= M;
    
    color.a = 1.0f;
    
    return color;
}