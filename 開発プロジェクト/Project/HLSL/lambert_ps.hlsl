#include "lambert.hlsli"

SamplerState linear_wrap : register(s0);
SamplerState linear_border : register(s1);

Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);

//Texture2D shadow_map : register(t4);

float4 main(PSInput input) : SV_TARGET
{
    // �e�N�Z���J���[�̎擾
    float4 color = color_map.Sample(linear_wrap, input.texcoord.xy);
    float alpha = 1.0f;

    // ���[���h��Ԃɂ�����@��
    float3 N = input.normal;
    // ���̋t�x�N�g��
    float3 L = normalize(-light_direction.xyz);
    // �����x�N�g��
    float3 E = normalize(input.world_position - camera_position);
    // ���˃x�N�g��
    float3 R = reflect(L, N);
  
    // �g�U����
    float3 diffuse = (dot(L, N)) * color.rgb * roughness_factor;
    // ���ʔ���
    float3 specular = max(0, pow(dot(R, E), 5)) * metallic_factor;

    return float4(Ambient(diffuse + specular), 1.0f) * float4(input.color.rgb, alpha);
}

//float4 main(VSOutput input) : SV_TARGET
//{
//    float4 color = color_map.Sample(set_sampler_state, input.texcoord);

//    float3 N = normalize(input.world_normal.xyz);

//    float3 L = normalize(-light_direction.xyz);

//    // diffuse
//    float3 diffuse = color.rgb * Diffuse(N, L);

//    // specular
//    float narrow = 200 * max(metallic_factor, 0.01f);
    
//    float3 E = normalize(camera_position.xyz - input.world_position.xyz);
//    float3 specular = Specular(N, L, E, float3(1, 1, 1), narrow);
 

//    return float4(Ambient(diffuse + specular), 1.0) * input.color;
//}
