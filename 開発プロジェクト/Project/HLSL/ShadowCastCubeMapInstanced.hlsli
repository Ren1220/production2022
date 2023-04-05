struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint instnace_id : SV_InstanceID;
};

struct HSInput
{
    float4 position : POSITION; // world
    float4 normal : NORMAL; // world
    float2 texcoord : TEXCOORD;
    uint instnace_id : SV_InstanceID;
};

struct DSInput
{
    float4 position : POSITION; // world
    float4 normal : NORMAL; // world
    float2 texcoord : TEXCOORD;
    uint instnace_id : SV_InstanceID;
};

struct DSConstantDataInput
{
    float edge_tess[3] : SV_TessFactor;
    float inside_tess : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

struct GSInput
{
    float4 position : SV_POSITION; // world
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION; // proj
    float4 world_position : POSITION;
    float2 texcoord : TEXCOORD;
    uint rt_index : SV_RenderTargetArrayIndex;
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

cbuffer CBufferCubeMapView : register(b3)
{
    row_major matrix cube_map_view[6];
    row_major matrix cube_map_proj;
    float4 cubemap_view_eye;
};