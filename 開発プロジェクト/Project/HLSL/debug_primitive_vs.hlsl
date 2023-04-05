#include "debug_primitive.hlsli"

PSInput main(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position.xyz, 1.0), mul(world, mul(view, projection)));
    return output;
}