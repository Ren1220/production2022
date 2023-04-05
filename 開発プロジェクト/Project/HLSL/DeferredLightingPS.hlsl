#include "Sprite.hlsli"
#include "DeferredLighting.hlsli"
#include "bidrectional_reflectance_distribution_function.hlsli"

Texture2D<float4> color_texture : register(t0);
Texture2D<float4> roughness_metallic_texture : register(t1);
Texture2D<float4> emmisive_texture : register(t2);
Texture2D<float4> occlusion_texture : register(t3);
Texture2D<float4> normal_texture : register(t4);
Texture2D<float4> position_texture : register(t5);

TextureCubeArray shadow_texture_array : register(t6);

SamplerState linear_border_sampler_state : register(s3); // �V���h�E�}�b�v�p

float4 main(PSInput input) : SV_TARGET
{
    float4 base_color           = color_texture.Sample(linear_wrap_sampler_state, input.texcoord);
    float3 world_position       = position_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    float2 roughness_metallic   = roughness_metallic_texture.Sample(linear_wrap_sampler_state, input.texcoord).rg;
    float2 occlusion            = occlusion_texture.Sample(linear_wrap_sampler_state, input.texcoord).rg;
    float3 emmisive             = emmisive_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    
    float3 normal = normal_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    normal = normal * 2.0f - 1.0f;
    
    // �@���̑��݂��Ȃ��s�N�Z���͂����ŏ������I��
    clip(abs(normal.x + normal.y + normal.z) < 0.01f);
    
    float roughness = roughness_metallic.r;
    float metallic = roughness_metallic.g;
    
    const float3 f0 = lerp(0.04, base_color.rgb, metallic);
    const float3 f90 = 1.0;
    const float alpha_roughness = roughness * roughness;
    const float3 c_diff = lerp(base_color.rgb, 0.0, metallic);
    
    const float3 V = normalize(camera_position.xyz - world_position);
    
    float3 diffuse = 0;
    float3 specular = 0;
    
    for (int li = 0; li < 10; ++li)
    {
        Light light = lights[li];
        
        float3 d = 0;
        float3 s = 0;
        
        if (light.luminance < 0.01f)
        {
            continue;
        }
        
        // �e���C�g�̃V�F�[�f�B���O���������[�v�ōs��
        float3 d_light_dir = normalize(-light.param.xyz);       

        float3 p_light_vec = world_position - light.param.xyz;

        // �����萔�o�b�t�@��param.w�̒l���O�𒴂���ꍇ
        // �����̃p�����[�^�Ƃ��Ĉ����A���̌�����_�����Ƃ݂Ȃ�
        float light_type = step(0.01f, light.param.w);
        float3 light_dir = lerp(d_light_dir, normalize(p_light_vec), light_type);

        float attenuation = light.param.w < 0.01f ?
                        1.0f :
                        max(0.0f, 1.0f - (length(p_light_vec) / light.param.w));

        
        // ���C�g�̕����x�N�g���ƕ\�ʂ̖@���x�N�g������A���C�g�̓��ˊp���v�Z���Ă��܂��B 
        // dot�֐��́A2�̃x�N�g���̓��ς��v�Z���܂��B 
        // ���̓��ς̌��ʂ́A���C�g�̓��ˊp�ɑΉ�����̂ŁA���ˊp���傫���قǑ傫�Ȓl�������܂��B 
        //float NoL = dot(float3(0,1 ,0), -light_dir);
        float NoL = dot(normal, -light_dir);

        // ���̍s�ł́A�\�ʂ̖@���x�N�g���ƃJ��������̎����x�N�g������A
        // �J�����̊p�x���v�Z���Ă��܂��B dot�֐��́A2�̃x�N�g���̓��ς��v�Z���܂��B 
        // ���̓��ς̌��ʂ́A�J�����̊p�x�ɑΉ�����̂ŁA�J��������̎�����
        // �\�ʂ̖@���ƕ��s�ł���قǑ傫�Ȓl�������܂��B 
        float NoV = max(0.0, dot(normal, V));
    
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float PI = 3.14159265358979;

            // LM
            float3 lm = light.color.rgb * light.luminance * attenuation;
        
            //const float3 R = reflect(-corrected_light_direction, normal);
            const float3 H = normalize(V - light_dir);
        
            float NoH = max(0.0, dot(normal, H));
            float HoV = max(0.0, dot(H, V));
        
            d += lm * NoL * Brdf_Lambertian(f0, f90, c_diff, HoV);
            s += lm * NoL * Brdf_Specular_GGX(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH) * 0.8f;
        }

        
        // �S���ʃV���h�E�̉e(�_����)
        if (light.param.w > 0.01)
        {
            // ���[���h��Ԃɂ�������̃x�N�g���̌���(�L���[�u�}�b�v��UV�l)
            float3 I = world_position - light.param.xyz;
            // ���߂��x�N�g���̋����̓��Ɖe�e�N�X�`���̐[�x�l���r���āA
            // �e�ɂȂ��Ă��邩���Z�o����B
            float bias = 0.98f;
            float depth = dot(I, I) * bias;
            
            {
                // ���݂̓_�����̃V���h�E�L���[�u�}�b�v���擾
                float3 shadow = shadow_texture_array.Sample(linear_wrap_sampler_state, float4(I, float(li)));
                // �s�N�Z���̐[�x�ƃ}�b�v�ɏ������܂ꂽ�[�x���r���e�ł��邩�ǂ������m�F����
                float f_shadow = (depth - shadow.r < 0.0) ? 1.0f : 0.0f; /*1.0f - saturate(depth / (light.param.w));*/
    
                // �e�ł���ꍇ�͕␳��������
                d *= f_shadow; //* saturate(depth / attenuation_);
                s *= f_shadow; //* saturate(depth / attenuation_);
            }
        }
        
        diffuse += d;
        specular += s;
    }
    
    // ���ȉA�e
    diffuse = lerp(diffuse, diffuse * occlusion.r, occlusion.g);
    specular = lerp(specular, specular * occlusion.r, occlusion.g);

    float3 Lo = diffuse + specular + emmisive;

    return float4(Lo, base_color.a);
}