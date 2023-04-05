
struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};

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

cbuffer CBufferPrimitive : register(b0)
{
    row_major float4x4 world;
    int material;
    bool has_tangent; // タンジェントが存在するか
    int skin;
    int pad;
}

cbuffer CBufferScene : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 camera_position;
}

static const uint PrimitiveMaxJoints = 512;

cbuffer CBufferJoint : register(b2)
{
    row_major float4x4 joint_matrices[PrimitiveMaxJoints];
}

cbuffer CBufferCubeMapView : register(b3)
{
    row_major matrix cube_map_view[6];
    row_major matrix cube_map_proj;
    float4 cubemap_view_eye;
};