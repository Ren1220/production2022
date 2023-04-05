#include "GeometryBuffer.hlsli"

/* --- ��` ---- */

struct TextureInfo
{
    int index;
    int texcoord;
};

struct NormalTextureInfo
{
    int index;
    int texcoord;
    float scale;
};

struct OcclusionTextureInfo
{
    int index;
    int texcoord;
    float strength;
};

struct PBRMetallicRoughness
{
    float4 basecolor_factor;
    TextureInfo basecolor_texture;
    float metallic_factor;
    float roughness_factor;
    TextureInfo metallic_roughness_texture;
};

struct MaterialConstants
{
    float3 emissive_factor;
    int alpha_mode; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
    float alpha_cutoff;
    bool double_sided;
    
    PBRMetallicRoughness pbr_metallic_roughness;
    NormalTextureInfo normal_texture;
    OcclusionTextureInfo occlusion_texture;
    TextureInfo emissive_texture;
};

// �\�����o�b�t�@
StructuredBuffer<MaterialConstants> materials : register(t0);

// �e�N�X�`��
Texture2D<float4> basecolor_texture : register(t1);
Texture2D<float4> metallic_roughness_texture : register(t2);
Texture2D<float4> normal_texture : register(t3);
Texture2D<float4> emissive_texture : register(t4);
Texture2D<float4> occlusion_texture : register(t5);

// �T���v���[�X�e�[�g
SamplerState point_wrap_sampler_state : register(s0);
SamplerState linear_wrap_sampler_state : register(s1);
SamplerState anisotropic_wrap_sampler_state : register(s2);

struct PSOutput
{
    float4 color     : SV_TARGET0; // 0 : �F
    float4 pbr       : SV_TARGET1; // 1 : �����x�[�X
    float4 emmisive  : SV_TARGET2; // 2 : ���Ȕ���
    float4 occlusion : SV_TARGET3; // 3 : �A�e
    float4 normal    : SV_TARGET4; // 4 : �@��
    float4 position  : SV_TARGET5; // 5 : 3D�ʒu
    float4 depth     : SV_TARGET6; // 6 : �[�x
};

/* --- �V�F�[�_�[�̃G���g���[�|�C���g ----*/

PSOutput main(PSInput input)
{

    const float Gamma = 2.2;
    
    const MaterialConstants m = materials[material];
    
    #if 1
    // ��{�F
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int has_basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    
    // �J���[�e�N�X�`�������݂��Ă���
    if (has_basecolor_texture > -1)
    {
        float4 sampled = basecolor_texture.Sample(anisotropic_wrap_sampler_state, input.texcoord);
           
        // �t�K���}�␳
        sampled.rgb = pow(sampled.rgb, Gamma);
        basecolor_factor *= sampled;
    }
    
    // �e���Ɣ��˗�
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int has_metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;

    // �e���Ɣ��˗��e�N�X�`�������݂��Ă���
    if (has_metallic_roughness_texture > -1)
    {
        float4 sampled = metallic_roughness_texture.Sample(linear_wrap_sampler_state, input.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
    }
    
    // ���Ȕ����F
    float3 emmisive_factor = m.emissive_factor;
    const int has_emmisive_texture = m.emissive_texture.index;

    // ���Ȕ����e�N�X�`�������݂��Ă���
    if (has_emmisive_texture > -1)
    {
        float4 sampled = emissive_texture.Sample(anisotropic_wrap_sampler_state, input.texcoord);
        
        sampled.rgb = pow(sampled.rgb, Gamma);
        emmisive_factor *= sampled.rgb;
    }
    
    // �A�e
    float occlusion_factor = 1.0;
    const int has_occulusion_texture = m.occlusion_texture.index;

    // �A�e�e�N�X�`�������݂��Ă���
    if (has_occulusion_texture > -1)
    {
        float4 sampled = occlusion_texture.Sample(linear_wrap_sampler_state, input.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
    
    float3 world_normal = normalize(input.world_normal.xyz);
    float3 world_tangent = normalize(input.world_tangent.xyz);
    float3 world_binormal = normalize(input.world_tangent.xyz);
    //float sigma = has_tangent ? input.world_tangent.w : 1.0;
    //world_tangent = normalize(world_tangent - world_normal * dot(world_normal, world_tangent));
    //float3 world_binormal = normalize(cross(world_normal, world_tangent) * sigma);
   
    const int has_normal_texture = m.normal_texture.index;
    
    // �@���e�N�X�`�������݂��Ă���
    if (has_normal_texture > -1)
    {
        float3 normal_factor = normal_texture.Sample(linear_wrap_sampler_state, input.texcoord).xyz;
        normal_factor = (normal_factor * 2.0f) - 1.0f;
        normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
        // �^���W�F���g��Ԃɕϊ�
        world_normal = normalize((normal_factor.x * world_tangent) + (normal_factor.y * world_binormal) + (normal_factor.z * world_normal));
    }
    
    PSOutput output = (PSOutput) 0;
    output.color = basecolor_factor;
    output.pbr = float4(roughness_factor, metallic_factor, 0, 1);
   	//-1 <---> +1 �� 0 <---> 1 ��R8G8B8A8�Ή�
    world_normal = world_normal * 0.5 + 0.5;
    output.normal = float4(world_normal, 1);
    output.emmisive = float4(emmisive_factor, 1);
    output.occlusion = float4(occlusion_factor, occlusion_strength, 0, 1);
    output.position = float4(input.world_position.xyz, 1.0f);
    float distance = length(input.world_position - camera_position);
    output.depth= float4(distance, 0, 0, 1);

    
    #else
    
    // ��{�F
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int has_basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    // �J���[�e�N�X�`�������݂��Ă���
    if (has_basecolor_texture > -1)
    {
        float4 sampled = basecolor_texture.Sample(anisotropic_wrap_sampler_state, input.texcoord);
        
        // �K���}�␳
        sampled.rgb = pow(sampled.rgb, Gamma);
        basecolor_factor *= sampled;
    }

    // �e���Ɣ��˗�
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int has_metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
    // ���Ȕ����F
    float3 emmisive_factor = m.emissive_factor;
    const int has_emmisive_texture = m.emissive_texture.index;
    // �A�e
    float occlusion_factor = 1.0;
    const int has_occulusion_texture = m.occlusion_texture.index;
    const float occlusion_strength = m.occlusion_texture.strength;
    
    float3 world_normal = float3(0, 1, 0);
    float3 world_tangent = float3(0, 0, 1);
    float3 world_binormal = float3(1, 0, 0);
   
    PSOutput output = (PSOutput) 0;
    output.color = basecolor_factor;
    output.pbr = float4(roughness_factor, metallic_factor, 0, 1);
   	//-1 <---> +1 �� 0 <---> 1 ��R8G8B8A8�Ή�
    world_normal = world_normal * 0.5 + 0.5;
    output.normal = float4(world_normal, 1);
    output.emmisive = float4(emmisive_factor, 1);
    output.occlusion = float4(occlusion_factor, occlusion_strength, 0, 1);
    output.position = float4(input.world_position.xyz, 1.0f);
    float distance = length(input.world_position - camera_position);
    output.depth = float4(distance, 0, 0, 1);
    
    #endif
    
    return output;
}