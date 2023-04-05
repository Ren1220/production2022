#include "cloud.hlsli"

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.position = mul(mul(input.position, view), projection);
    output.texcoord = input.texcoord;

    return output;
}