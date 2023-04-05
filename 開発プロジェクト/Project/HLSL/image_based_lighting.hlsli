#ifndef __IMAGE_BASED_LIGHTING__
#define __IMAGE_BASED_LIGHTING__

Texture2D skybox : register(t32);
TextureCube diffuse_iem : register(t33);
TextureCube specular_pmrem : register(t34);
Texture2D lut_ggx : register(t35);

// �T���v���[�X�e�[�g
SamplerState point_wrap_sampler_state : register(s0);
SamplerState linear_wrap_sampler_state : register(s1);
SamplerState anisotropic_wrap_sampler_state : register(s2);

float4 SampleLutGGX(float2 brdf_sample_point)
{
    return lut_ggx.Sample(linear_wrap_sampler_state, brdf_sample_point);
}


float4 SampleSkybox(float3 v, float roughness)
{
    const float PI = 3.14159265358979;
    uint width, height, number_of_levels;
    skybox.GetDimensions(0, width, height, number_of_levels);
    
    float lod = roughness * float(number_of_levels - 1);
    
    v = normalize(v);
    
    // Blinn/Newell Latitude Mapping
    float2 sample_point;
    sample_point.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    sample_point.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
    return skybox.SampleLevel(linear_wrap_sampler_state, sample_point, lod);
}

float4 SampleDiffuseIem(float3 v)
{
    return diffuse_iem.Sample(linear_wrap_sampler_state, v);
}

float4 SampleSpecularPmrem(float3 v, float roughness)
{
    uint width, height, number_of_levels;
    specular_pmrem.GetDimensions(0, width, height, number_of_levels);
    
    float lod = roughness * float(number_of_levels - 1);
    
    return specular_pmrem.SampleLevel(linear_wrap_sampler_state, v, lod);

}

#endif