
struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
    float4 texcoord4 : TEXCOORD4;
    float4 texcoord5 : TEXCOORD5;
    float4 texcoord6 : TEXCOORD6;
    float4 texcoord7 : TEXCOORD7;
};

cbuffer BlurWeight : register(b5)
{
    float4 blur_weight[2];
}