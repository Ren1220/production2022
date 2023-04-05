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

SamplerState linear_border_sampler_state : register(s3); // シャドウマップ用

float4 main(PSInput input) : SV_TARGET
{
    float4 base_color           = color_texture.Sample(linear_wrap_sampler_state, input.texcoord);
    float3 world_position       = position_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    float2 roughness_metallic   = roughness_metallic_texture.Sample(linear_wrap_sampler_state, input.texcoord).rg;
    float2 occlusion            = occlusion_texture.Sample(linear_wrap_sampler_state, input.texcoord).rg;
    float3 emmisive             = emmisive_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    
    float3 normal = normal_texture.Sample(linear_wrap_sampler_state, input.texcoord).rgb;
    normal = normal * 2.0f - 1.0f;
    
    // 法線の存在しないピクセルはここで処理を終了
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
        
        // 各ライトのシェーディング処理をループで行う
        float3 d_light_dir = normalize(-light.param.xyz);       

        float3 p_light_vec = world_position - light.param.xyz;

        // 光源定数バッファのparam.wの値が０を超える場合
        // 減衰のパラメータとして扱い、この光源を点光源とみなす
        float light_type = step(0.01f, light.param.w);
        float3 light_dir = lerp(d_light_dir, normalize(p_light_vec), light_type);

        float attenuation = light.param.w < 0.01f ?
                        1.0f :
                        max(0.0f, 1.0f - (length(p_light_vec) / light.param.w));

        
        // ライトの方向ベクトルと表面の法線ベクトルから、ライトの入射角を計算しています。 
        // dot関数は、2つのベクトルの内積を計算します。 
        // この内積の結果は、ライトの入射角に対応するので、入射角が大きいほど大きな値が得られます。 
        //float NoL = dot(float3(0,1 ,0), -light_dir);
        float NoL = dot(normal, -light_dir);

        // この行では、表面の法線ベクトルとカメラからの視線ベクトルから、
        // カメラの角度を計算しています。 dot関数は、2つのベクトルの内積を計算します。 
        // この内積の結果は、カメラの角度に対応するので、カメラからの視線が
        // 表面の法線と平行であるほど大きな値が得られます。 
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

        
        // 全方位シャドウの影(点光源)
        if (light.param.w > 0.01)
        {
            // ワールド空間における光のベクトルの向き(キューブマップのUV値)
            float3 I = world_position - light.param.xyz;
            // 求めたベクトルの距離の二乗と影テクスチャの深度値を比較して、
            // 影になっているかを算出する。
            float bias = 0.98f;
            float depth = dot(I, I) * bias;
            
            {
                // 現在の点光源のシャドウキューブマップを取得
                float3 shadow = shadow_texture_array.Sample(linear_wrap_sampler_state, float4(I, float(li)));
                // ピクセルの深度とマップに書き込まれた深度を比較し影であるかどうかを確認する
                float f_shadow = (depth - shadow.r < 0.0) ? 1.0f : 0.0f; /*1.0f - saturate(depth / (light.param.w));*/
    
                // 影である場合は補正がかかる
                d *= f_shadow; //* saturate(depth / attenuation_);
                s *= f_shadow; //* saturate(depth / attenuation_);
            }
        }
        
        diffuse += d;
        specular += s;
    }
    
    // 自己陰影
    diffuse = lerp(diffuse, diffuse * occlusion.r, occlusion.g);
    specular = lerp(specular, specular * occlusion.r, occlusion.g);

    float3 Lo = diffuse + specular + emmisive;

    return float4(Lo, base_color.a);
}