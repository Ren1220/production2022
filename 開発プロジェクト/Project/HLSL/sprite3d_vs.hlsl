#include "sprite3d.hlsli"

VSOutput main(PSInput input)
{
    VSOutput output;
    output.position = mul(input.position, mul(world, mul(view, projection)));
    output.color = input.color * material_color;
    return output;
}