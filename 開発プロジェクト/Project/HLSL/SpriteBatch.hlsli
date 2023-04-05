
struct VSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    row_major float4x4 ndc_transform : NDC_TRANSFORM;
    float4 texcoord_transform : TEXCOORD_TRANSFORM;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};