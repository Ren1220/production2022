
struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;      // WVP行列(透視投影射影空間座標)
    float4 world_position : POSTION;    // ワールド座標
    float3 tangent : TANGENT;           // 接線   X
    float3 binormal : BINORMAL;         // 従法線 Y
    float3 normal : NORMAL;             // 法線   Z
    float4 texcoord : TEXCOORD;         // UV座標
    float4 color : COLOR;               // 入力色
    //float3 shadow_texcoord : TEXCOORD1; // シャドウマップ用
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
