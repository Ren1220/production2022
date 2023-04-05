struct VSInput
{
    float4 position : Position;
};

struct VSOutput
{
    float4 position : Position;
};

#define NUM_CONTROL_POINTS 3
// �n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y�p�̏o�̓p�����[�^
struct ConstantHSOutput
{
    float edge_tess_factor[3] : SV_TessFactor;             //  �p�b�`�̃G�b�W�̃e�b�Z���[�V�����W��
    float inside_tess_factor[1]: SV_InsideTessFactor;    //  �p�b�`�����̃e�b�Z���[�V�����W��
};

// �n���V�F�[�_�[�̃R���g���[���|�C���g�t�F�[�Y�p�̏o�̓p�����[�^
struct HSOutput
{
    float4 position : Position;
};

// �h���C���V�F�[�_�[�̏o�̓p�����[�^
struct DSOutput
{
    float4 position : SV_Position;
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
    row_major float4x4 view_inverse;
    float4 light_direction;
    float4 camera_position;
}

cbuffer CBufferTessellation : register(b2)
{
    float edge_factor;
    float inside_tess_factor;
    float2 pad2;
};

Texture2D color_map : register(t0);

SamplerState linerWrap : register(s0);
SamplerState linerBorder : register(s1);

