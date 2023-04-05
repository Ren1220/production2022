/* --- �t�@�C���̃C���N���[�h ---- */

#include "light.hlsli" // �_�����A�X�|�b�g���C�g�i���j

/* --- �\���̂̒�` ---- */

struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float4 world_position : POSTION;
    float4 world_normal : NORMAL;
    float3 shadow_texcoord : TEXCOORD1; // �V���h�E�}�b�v�p
    float4 projection_texcoord : TEXCOORD2;
};

/* --- �萔�o�b�t�@ ---- */

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
    row_major float4x4 view_inverse;
    float4 light_direction;
    float4 camera_position;
}

cbuffer CBufferLight : register(b3)
{
    PointLight point_lights[3];
}

cbuffer CBufferProjectionTexture : register(b4)
{
    row_major float4x4 projection_texture;
}