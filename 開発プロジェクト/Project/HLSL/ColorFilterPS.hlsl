
#include "Sprite.hlsli"
#include "ColorFilter.hlsli"

Texture2D color_texture : register(t0);
SamplerState color_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = color_texture.Sample(color_sampler_state, input.texcoord) * input.color;

    // RGB -> HSV
    color.rgb = RGBToHSV(color.rgb);
    
    // 色相
    color.r += hue_shift;
    
    // 彩度
    color.g *= saturation;
    
    // 明度
    color.b *= brightness;
    
    // HSV -> RGB
    color.rgb = HSVToRGB(color.rgb);
    
    // コントラスト
    color.rgb = (color.rgb - 0.5f) * contrast + 0.5f;
    
    // ガンマ補正
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}