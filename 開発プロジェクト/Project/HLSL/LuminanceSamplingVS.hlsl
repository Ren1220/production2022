#include "LuminanceSampling.hlsli"

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;
    output.position = input.position;
    output.texcoord = input.texcoord;
    return output;
}