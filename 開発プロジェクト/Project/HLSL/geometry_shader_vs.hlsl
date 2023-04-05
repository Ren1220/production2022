#include "geometry_shader.hlsli"

GSInput main(VSInput input)
{
    GSInput output;

    output.position = input.position;
    output.texcoord = input.texcoord;
    output.size = input.size;
    output.color = input.color;
    output.dissolve = input.dissolve;

    return output;
}