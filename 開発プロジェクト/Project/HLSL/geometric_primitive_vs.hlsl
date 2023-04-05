#include "geometric_primitive.hlsli"

VSOutput main(VSInput input )
{
    VSOutput output;
    output.position = mul(input.position, mul(world, mul(view, projection)));
    return output;
}