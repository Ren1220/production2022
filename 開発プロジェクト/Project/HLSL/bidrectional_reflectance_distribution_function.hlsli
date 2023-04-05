
#include "image_based_lighting.hlsli"

// 双方向反射率分布関数

#ifndef __BIDRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__
#define __BIDRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__

//https://light11.hatenadiary.com/entry/2020/03/05/220957
// https://qiita.com/emadurandal/items/76348ad118c36317ec5c
// https://github.com/KhronosGroup/glTF-Sample-Viewer/tree/glTF-WebGL-PBR

// BRDF : 入ってきた光のうちどれだけ反射するか

// f0 : 法線方向から入射する光の反射率
// f90 : グレージング角から入射する光の反射率　( グレージング角 : 平面と入射光のなす角)
// VoH : 視線単位ベクトルとハーフベクトルの内積値

// フレネル項(視線ベクトルと法線の角度に応じて反射率が変わる現象)の近似式
float3 F_Schlick(float3 f0, float3 f90, float VoH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}



// 幾何減衰項
float V_GGX(float NOL, float NOV, float alpha_roughness)
{
    float alpha_roughness_sq = alpha_roughness * alpha_roughness;
    
    //float ggxv = NOL * sqrt(NOV * NOV * (1.0 - alpha_roughness_sq) + alpha_roughness_sq);
    //float ggxl = NOV * sqrt(NOL * NOL * (1.0 - alpha_roughness_sq) + alpha_roughness_sq);

    //float ggx = ggxv + ggxl;

    //return (ggx > 0.0) ? 0.5 / ggx : 0.0;

    // https://github.com/KhronosGroup/glTF-Sample-Viewer/tree/glTF-WebGL-PBR

    float NdotL2 = NOL * NOL;
    float NdotV2 = NOV * NOV;
    float v = (-1.0 + sqrt(alpha_roughness_sq * (1.0 - NdotL2) / NdotL2 + 1.)) * 0.5;
    float l = (-1.0 + sqrt(alpha_roughness_sq * (1.0 - NdotV2) / NdotV2 + 1.)) * 0.5;
    return (1.0 / max((1.0 + v + l), 0.000001));
}

// 法線分布関数
float D_GGX(float NOH, float alpha_roughness)
{
    const float PI = 3.14159265358979;
    float alpha_roughness_sq = alpha_roughness * alpha_roughness;
    float f = (NOH * NOH) * (alpha_roughness_sq - 1.0) + 1.0;
    
    return alpha_roughness_sq / (PI * f * f);
}

float3 Brdf_Lambertian(float3 f0, float3 f90, float3 diffuse_color, float VoH)
{
    const float PI = 3.14159265358979;
    return (1.0 - F_Schlick(f0, f90, VoH)) * (diffuse_color / PI);
}

float3 Brdf_Specular_GGX(float3 f0, float3 f90, float alpha_roughness,
    float VOH, float NOL, float NOV, float NOH)
{
    float3 F = F_Schlick(f0, f90, VOH);
    float Vis = V_GGX(NOL, NOV, alpha_roughness);
    float D = D_GGX(NOH, alpha_roughness);

    return F * Vis * D;
}

float3 IBL_RadianceLabertation(float3 N, float3 V, float roughness, float3 diffuse_color, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdf_sample_point = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = SampleLutGGX(brdf_sample_point).rg;
    
    float3 irradiance = SampleDiffuseIem(N).rgb;
    
    float3 fr = max(1.0 - roughness, f0) - f0;
    
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;
    
    float ems = (1.0 - (f_ab.x + f_ab.y));
    float3 f_avg = (f0 + (1.0 - f0 / 21.0));
    float3 fms_ems = ems * fss_ess * f_avg / (1.0 - f_avg * ems);
    float3 k_d = diffuse_color * (1.0 - fss_ess + fms_ems);
    
    return (fms_ems + k_d) * irradiance;
}

float3 IBL_RadianceGGX(float3 N, float3 V, float roughness, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdf_sample_point = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = SampleLutGGX(brdf_sample_point).rg;
    
    float3 R = normalize(reflect(-V, N));
    float3 specular_light = SampleSpecularPmrem(R, roughness).rgb;
    
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;
        
    return specular_light * fss_ess;
}

#endif