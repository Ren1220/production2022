#include "phong.hlsli"

SamplerState linear_wrap : register(s0);
SamplerState linear_border : register(s1);

Texture2D color_map : register(t0);
Texture2D projection_map : register(t1);

float4 main(PSInput input) : SV_TARGET
{
    // �e�N�Z���J���[�̎擾
    float4 main_color = color_map.Sample(linear_wrap, input.texcoord);

    // �ˉe�e�N�X�`���̃J���[�̎擾
    // �ˉeUV���W
    float4 projection_texcoord = mul(input.world_position, projection_texture);
    // ���K��
    projection_texcoord /= projection_texcoord.w;

    float4 projection_color = 0;
    if (projection_texcoord.z == saturate(projection_texcoord.z))
    {
        // �ˉe����ʒu�𒲐�
        projection_texcoord = float4(projection_texcoord.x * 0.5f + 0.5f, 0.5f - 0.5f * projection_texcoord.y, projection_texcoord.z, 1.0f);
        projection_color = projection_map.Sample(linear_border, projection_texcoord.xy);
    }


    // �F�̍���
    //float alpha = main_color.a + projection_color.a;
    //float3 color = main_color.rgb * (main_color.a / alpha) + projection_color.rgb * (projection_color.a / alpha);
    float4 color = (main_color.a > projection_color.a) ? main_color : projection_color;

    // ���[���h��Ԃɂ�����@��
    float3 N = normalize(input.world_normal.xyz);

    // ���̋t�x�N�g��
    float3 L = normalize(light_direction.xyz);

    // ���K���ςݎ��_�x�N�g��
    float3 E = normalize(camera_position.xyz - input.world_position.xyz);

    // ���˃x�N�g��
    float3 R = reflect(-L, N);

    // �g�U����
    float3 diffuse = (dot(L, N) /** 0.5f + 0.5f*/) * color.rgb * roughness_factor;

    // ���ʔ���
    float3 specular = max(0, pow(dot(R, E), 100)) * metallic_factor;

    // �_�����̌v�Z
    float3 point_light = (float3) 0;

    for (int i = 0; i < 3; i++)
    {
        // �_�����Ƃ̋���
        float3 distance = length(input.world_position.xyz - point_lights[i].position.xyz);

        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float influence = 1.0f - 1.0f / point_lights[i].range * distance;

        // �e�������}�C�i�X�Ȃ�Ȃ��悤�ɂ���
        influence = max(influence, 0.0f);

        // �e�������w���֐��I�ɂ���
        //influence = pow(influence, 3.0f);
        
        // �_�����̃x�N�g��
        float3 PL = normalize(point_lights[i].position.xyz - input.world_position.xyz);


        // �_�����̔��˃x�N�g��
        float3 PR = reflect(-PL, N);

        // �_�����̊g�U����
        float3 PD = dot(PL, N) * color.rgb * roughness_factor * influence * roughness_factor ;

        // �_�����̋��ʔ���
        float3 PS = max(0, pow(dot(PR, E), 100)) * influence * metallic_factor;

        diffuse += PD;
        specular += PS;
    }

    // ����
    float3 ambient = float3(0.2f, 0.2f, 0.2f);

    return float4(diffuse + specular + ambient, 1.0f) * input.color; //float4(input.color.rgb, alpha / 2.0f);
}
