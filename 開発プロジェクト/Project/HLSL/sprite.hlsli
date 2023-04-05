
struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CBufferMaterial : register(b4)
{
    float4 material_color;
};