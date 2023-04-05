#include "GLTFModel.hlsli"
#include "bidrectional_reflectance_distribution_function.hlsli"

/* --- 定義 ---- */

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
    
    PBRMetallicRoughness    pbr_metallic_roughness;
    NormalTextureInfo       normal_texture;
    OcclusionTextureInfo    occlusion_texture;
    TextureInfo             emissive_texture;
};

// 構造化バッファ
StructuredBuffer<MaterialConstants> materials   : register(t0);

// テクスチャ
Texture2D<float4> basecolor_texture             : register(t1);
Texture2D<float4> metallic_roughness_texture    : register(t2);
Texture2D<float4> normal_texture                : register(t3);
Texture2D<float4> emissive_texture              : register(t4);
Texture2D<float4> occlusion_texture             : register(t5);


/* --- シェーダーのエントリーポイント ----*/

float4 main(PSInput input) : SV_TARGET
{
    const float Gamma = 2.2;
    
    const MaterialConstants m = materials[material];
    
    // 基本色
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int has_basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;

    // カラーテクスチャが存在している
    if(has_basecolor_texture > -1)
    {
        float4 sampled = basecolor_texture.Sample(anisotropic_wrap_sampler_state, input.texcoord);
        
        // 逆ガンマ補正
        sampled.rgb = pow(sampled.rgb, Gamma);
        basecolor_factor *= sampled;
    }
    
    
    // 自己発光色
    float3 emmisive_factor = m.emissive_factor;
    const int has_emmisive_texture = m.emissive_texture.index;

    // 自己発光テクスチャが存在している
    if(has_emmisive_texture > -1)
    {
        float4 sampled = emissive_texture.Sample(anisotropic_wrap_sampler_state, input.texcoord);
        
        sampled.rgb = pow(sampled.rgb, Gamma);
        emmisive_factor *= sampled.rgb;
    }
    
    // 粗さと反射率
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int has_metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;

    // 粗さと反射率テクスチャが存在している
    if (has_metallic_roughness_texture > -1)
    {
        float4 sampled = metallic_roughness_texture.Sample(linear_wrap_sampler_state, input.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
    }
    
    // 陰影
    float occlusion_factor = 1.0;
    const int has_occulusion_texture = m.occlusion_texture.index;

    // 陰影テクスチャが存在している
    if(has_occulusion_texture > -1)
    {
        float4 sampled = occlusion_texture.Sample(linear_wrap_sampler_state, input.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
    
    const float3 f0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
    const float3 f90 = 1.0;
    const float alpha_roughness = roughness_factor * roughness_factor;
    const float3 c_diff = lerp(basecolor_factor.rgb, 0.0, metallic_factor);
    
    const float3 P = input.world_position.xyz + mul(float4(0, 3, 0, 1), world).xyz; // + float3(0, 3, 0)
    const float3 V = normalize(camera_position.xyz - input.world_position.xyz + mul(float4(0, 3, 0, 1), world).xyz);
        
    float3 N = normalize(input.world_normal.xyz);
    float3 T = has_tangent ? normalize(input.world_tangent.xyz) : float3(1, 0, 0);
    float sigma = has_tangent ? input.world_tangent.w : 1.0;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    
    const int has_normal_texture = m.normal_texture.index;
    
    // 法線テクスチャが存在している
    if(has_normal_texture > -1)
    {
        float4 sampled = normal_texture.Sample(linear_wrap_sampler_state, input.texcoord);
        float3 normal_factor = sampled.xyz;
        normal_factor = (normal_factor * 2.0) - 1.0;
        normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
        N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
    }

    float3 diffuse = 0;
    float3 specular = 0;
    
    // 各ライトのシェーディング処理をループで行う
    float3 L = normalize(-float3(1, -1, -1));
    //float3 Li = float3(1.0, 1.0, 1.0); // 上げた
    float3 Li = float3(5.0, 5.0, 5.0); // 上げた
    const float NoL = max(0.0, dot(N, L));
    const float NoV = max(0.0, dot(N, V));
    
    if(NoL > 0.0 || NoV > 0.0)
    {
        const float3 R = reflect(-L, N);
        const float3 H = normalize(V + L);
        
        const float NoH = max(0.0, dot(N, H));
        const float HoV = max(0.0, dot(H, V));
        
        diffuse += Li * NoL * Brdf_Lambertian(f0, f90, c_diff, HoV);
        specular += Li * NoL * Brdf_Specular_GGX(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
    }

    // IBL Lighting
    #if 0
    diffuse += IBL_RadianceLabertation(N, V, roughness_factor, c_diff, f0);
    specular += IBL_RadianceGGX(N, V, roughness_factor, f0);
    #endif
    
    float3 emmisive = emmisive_factor;
    diffuse = lerp(diffuse, diffuse * occlusion_factor, occlusion_strength);
    specular = lerp(specular, specular * occlusion_factor, occlusion_strength);
    
    float3 Lo = diffuse + specular + emmisive;
    return float4(Lo, basecolor_factor.a);
}