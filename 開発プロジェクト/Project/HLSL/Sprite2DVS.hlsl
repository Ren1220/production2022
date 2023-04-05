#include "Sprite.hlsli"

PSInput main(VSInput input)
{
    PSInput output;
    output.position = input.position;
    output.color = input.color;
    output.texcoord = input.texcoord;

    return output;
}