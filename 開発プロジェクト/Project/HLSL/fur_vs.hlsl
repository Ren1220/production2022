#include "fur.hlsli"

PSInput main(VSInput input)
{
    PSInput output;
    
    // get view position
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// get world position
    output.world_position = mul(input.position, world);

	// get world normal
    input.normal.w = 0;
    output.world_normal = normalize(mul(input.normal, world));

    output.color = material_color;

    output.texcoord = input.texcoord;

    return output;
}