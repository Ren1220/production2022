struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

cbuffer CBufferObject : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
}

cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 light_direction;
    float4 camera_position;
}
