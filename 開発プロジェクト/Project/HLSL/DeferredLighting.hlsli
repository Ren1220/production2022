cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 camera_position;
}

struct Light
{
    float4 param;
    float3 color;
    float  luminance;
};

cbuffer CBufferLight : register(b3)
{
    Light lights[10];
}