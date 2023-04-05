
#include "Sprite.hlsli"
#include "ColorFilter.hlsli"

Texture2D color_texture : register(t0);
SamplerState color_sampler_state : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 color = color_texture.Sample(color_sampler_state, input.texcoord) * input.color;

    // RGB -> HSV
    color.rgb = RGBToHSV(color.rgb);
    
    // �F��
    color.r += hue_shift;
    
    // �ʓx
    color.g *= saturation;
    
    // ���x
    color.b *= brightness;
    
    // HSV -> RGB
    color.rgb = HSVToRGB(color.rgb);
    
    // �R���g���X�g
    color.rgb = (color.rgb - 0.5f) * contrast + 0.5f;
    
    // �K���}�␳
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}