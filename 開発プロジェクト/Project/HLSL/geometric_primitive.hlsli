struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

cbuffer CBufferObject : register(b0)
{
    row_major float4x4 world;
    float4 color;
};

cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 camera_position;
}
