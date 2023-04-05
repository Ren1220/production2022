
struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;      // WVP�s��(�������e�ˉe��ԍ��W)
    float4 world_position : POSTION;    // ���[���h���W
    float3 tangent : TANGENT;           // �ڐ�   X
    float3 binormal : BINORMAL;         // �]�@�� Y
    float3 normal : NORMAL;             // �@��   Z
    float4 texcoord : TEXCOORD;         // UV���W
    float4 color : COLOR;               // ���͐F
    //float3 shadow_texcoord : TEXCOORD1; // �V���h�E�}�b�v�p
};

cbuffer CBufferScene: register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 view_inverse;
    float4 light_direction;
    float4 camera_position;
}

cbuffer CBufferObject : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
    float metallic_factor;
    float roughness_factor;
    float2 pad;
}

//cbuffer CBufferShadowMap : register(b4)
//{
//    row_major float4x4 light_view_projection;
//    float3 shadow_color;
//    float shadow_bias;
//    int on_shadow_map;
//    float3 pad4;
//}

#include "function.hlsli"
