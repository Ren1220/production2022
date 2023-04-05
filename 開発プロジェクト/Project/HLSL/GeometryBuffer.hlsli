
struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORDS;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float4 world_binormal : BINORMAL;
    float2 texcoord : TEXCOORD;
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