struct VSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float2 size : SIZE;
    float4 color : COLOR;

};

struct GSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float2 size : SIZE;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR0;
};

cbuffer CBufferObject : register(b0)
{

    row_major float4x4 world;
    float4 material_color;
    float metallic_factor;
    float roughness_factor;
    float2 pad;
}

cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float4 light_direction;
    float4 camera_position;
}