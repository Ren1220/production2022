#include "frame_buffer.hlsli"

PSInput main( VSInput input)
{
    PSInput output;
    output.position = input.position;
    output.texcoord = input.texcoord;

    return output;
}