
struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
    uint instnace_id : SV_InstanceID;
};

struct HSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint instnace_id : INSTANCE_ID;
};

struct DSConstantDataInput
{
    float edge_tess[3] : SV_TessFactor;
    float inside_tess : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

struct DSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint instnace_id : INSTANCE_ID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float4 world_binormal : BINORMAL;
    float3 texcoord : TEXCOORD;
    uint instance_id : INSTANCE_ID;
};

cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 camera_position;
}

struct StructuredBufferPrimitive
{
    row_major float4x4 world;
    uint material_index;
    int is_displacement;
    float displacement_scale;
};