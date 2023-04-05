#include "debug_grid.hlsli"

//PSInput main(VSInput input)
//{
//    PSInput output;
//    output.position = mul(input.position, mul(world, mul(view, projection)));
//    output.color = input.color * material_color;
//    return output;
//}

GSInput main(VSInput input)
{
    GSInput output;
    //output.position = mul(input.position, mul(world, mul(view, projection)));
    output.position = input.position;
    output.width = input.width;
    output.color = input.color * material_color;
    return output;
}