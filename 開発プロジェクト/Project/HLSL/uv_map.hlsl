#include "uv_map.hlsli"

PSInput main( float2 texcoord : TEXCOORD )
{
    PSInput output;

    output.position = float4(texcoord,0.0f, 1.0);
    //output.position = float4(texcoord.x * 2.0f - 1.0f, 1.0f - 2.0f * texcoord.y  ,0.0f, 1.0);
    output.texcoord = texcoord;

    return output;
}