#include "tessellation.hlsli"

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = input.position;

    return output;
}