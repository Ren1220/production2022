#include "phong.hlsli"

SamplerState linear_wrap : register(s0);
SamplerState linear_border : register(s1);

Texture2D color_map : register(t0);
Texture2D projection_map : register(t1);

float4 main(PSInput input) : SV_TARGET
{
    // テクセルカラーの取得
    float4 main_color = color_map.Sample(linear_wrap, input.texcoord);

    // 射影テクスチャのカラーの取得
    // 射影UV座標
    float4 projection_texcoord = mul(input.world_position, projection_texture);
    // 正規化
    projection_texcoord /= projection_texcoord.w;

    float4 projection_color = 0;
    if (projection_texcoord.z == saturate(projection_texcoord.z))
    {
        // 射影する位置を調整
        projection_texcoord = float4(projection_texcoord.x * 0.5f + 0.5f, 0.5f - 0.5f * projection_texcoord.y, projection_texcoord.z, 1.0f);
        projection_color = projection_map.Sample(linear_border, projection_texcoord.xy);
    }


    // 色の合成
    //float alpha = main_color.a + projection_color.a;
    //float3 color = main_color.rgb * (main_color.a / alpha) + projection_color.rgb * (projection_color.a / alpha);
    float4 color = (main_color.a > projection_color.a) ? main_color : projection_color;

    // ワールド空間における法線
    float3 N = normalize(input.world_normal.xyz);

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
        float3 PD = dot(PL, N) * color.rgb * roughness_factor * influence * roughness_factor ;

        // 点光源の鏡面反射
        float3 PS = max(0, pow(dot(PR, E), 100)) * influence * metallic_factor;

        diffuse += PD;
        specular += PS;
    }

    // 環境光
    float3 ambient = float3(0.2f, 0.2f, 0.2f);

    return float4(diffuse + specular + ambient, 1.0f) * input.color; //float4(input.color.rgb, alpha / 2.0f);
}
