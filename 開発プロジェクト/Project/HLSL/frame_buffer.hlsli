struct VSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer PostEffect : register(b3)
{
    float contrast;
    float3 pad0;
}