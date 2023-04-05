#include "snow_field.hlsli"

SamplerState linear_wrap : register(s0);
SamplerState linear_border : register(s1);

Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);

float4 main(PSInput input) : SV_TARGET
{
    // テクセルカラーの取得
    float4 color = color_map.Sample(linear_wrap, input.texcoord);

    // ワールド空間における法線
    //float3 N = normalize(input.world_normal.xyz);
    //N = normalize(N * normal_map.Sample(linear_wrap, input.texcoord).r);

    float3 N = { 0.0f, saturate(normal_map.Sample(linear_wrap, input.projection_texcoord.xy).r), 0.0f };

    // 光の逆ベクトル
    float3 L = normalize(light_direction.xyz);

    // 正規化済み視点ベクトル
    float3 E = normalize(camera_position.xyz - input.world_position.xyz);

    // 反射ベクトル
    float3 R = reflect(-L, N);

    // 拡散反射
    float3 diffuse = (dot(L, N) /** 0.5f + 0.5f*/) * color.rgb * roughness_factor;

    // 鏡面反射
    float3 specular = max(0, pow(dot(R, E), 100)) * metallic_factor;



    // 点光源の計算
    float3 point_light = (float3) 0;

    for (int i = 0; i < 3; i++)
    {
        // 点光源との距離
        float3 distance = length(input.world_position.xyz - point_lights[i].position.xyz);

        // 影響率は距離に比例して小さくなっていく
        float influence = 1.0f - 1.0f / point_lights[i].range * distance;

        // 影響率がマイナスならないようにする
        influence = max(influence, 0.0f);

        // 影響率を指数関数的にする
        //influence = pow(influence, 3.0f);
        
        // 点光源のベクトル
        float3 PL = normalize(point_lights[i].position.xyz - input.world_position.xyz);


        // 点光源の反射ベクトル
        float3 PR = reflect(-PL, N);

        // 点光源の拡散反射
        float3 PD = dot(PL, N) * color.rgb * roughness_factor * influence * roughness_factor;

        // 点光源の鏡面反射
        float3 PS = max(0, pow(dot(PR, E), 100)) * influence * metallic_factor;

        diffuse += PD;
        specular += PS;
    }

    // 環境光
    float3 ambient = float3(0.2f, 0.2f, 0.2f);

    return float4(diffuse + specular + ambient, 1.0) * input.color;
}
