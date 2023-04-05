#include "snow_field.hlsli"

SamplerState linear_wrap : register(s0);

Texture2D normal_map : register(t1);

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    // パースペクティブ座標
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// ワールド座標
    output.world_position = mul(input.position, world);


	// ワールド法線
    input.normal.w = 0;
    output.world_normal = normalize(mul(input.normal, world));

    output.color = material_color;

    output.texcoord = input.texcoord;

    output.projection_texcoord = mul(output.world_position, svp);
    output.projection_texcoord /= output.projection_texcoord.w;
    output.projection_texcoord.y = -output.projection_texcoord.y;
    output.projection_texcoord.xy = 0.5f * output.projection_texcoord.xy + 0.5f;


    return output;
}