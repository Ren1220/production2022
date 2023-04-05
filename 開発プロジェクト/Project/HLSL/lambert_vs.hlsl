#include "lambert.hlsli"

PSInput main(VSInput input)
{
    PSInput output;
    
    // WVP座標変換
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// ワールド座標変換
    output.world_position = mul(input.position, world);

    // 法線情報の作成
    output.normal = normalize(mul(float4(input.normal.xyz, 1), world));
    output.binormal = float3(0.0f, 1.0f, 0.001f); // 仮の上ベクトル
    output.tangent = normalize(cross(output.binormal, output.normal)); // 外積
    output.binormal = normalize(cross(output.normal, output.tangent)); // 外積

    // 入力色
    output.color = material_color;
        
    // UV座標
    output.texcoord = float4(input.texcoord, 0, 0);

#if 0
    // シャドウマップ用のパラメータ計算
    {
        // ライトの位置から見たＮＤＣ座標を算出
        float4 wvp_position = mul(input.position, mul(world, light_view_projection));

        // ＮＤＣ座標からＵＶ座標を算出
        wvp_position /= wvp_position.w;
        wvp_position.y = -wvp_position.y;
        wvp_position.xy = 0.5f * wvp_position.xy + 0.5f;

        output.shadow_texcoord = wvp_position.xyz;
    }
#endif

    return output;
}