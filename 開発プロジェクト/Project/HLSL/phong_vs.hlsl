#include "phong.hlsli"

PSInput main(VSInput input)
{
    PSInput output = (PSInput)0;
    
    // パースペクティブ座標
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// ワールド座標
    output.world_position = mul(input.position, world);
    

	// ワールド法線
    input.normal.w = 0;
    output.world_normal = normalize(mul(input.normal, world));

    output.color = material_color;

    output.texcoord = input.texcoord;

    // 射影UV座標
    output.projection_texcoord = mul(output.world_position, projection_texture);
    // 正規化
    output.projection_texcoord /= output.projection_texcoord.w;

    // 射影する位置を調整
    output.projection_texcoord = float4(output.projection_texcoord.x * 0.5f + 0.5f, 0.5f - 0.5f * output.projection_texcoord.y, output.projection_texcoord.z, 1.0f);

	return output;
}