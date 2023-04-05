#include "phong.hlsli"

PSInput main(VSInput input)
{
    PSInput output = (PSInput)0;
    
    // �p�[�X�y�N�e�B�u���W
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// ���[���h���W
    output.world_position = mul(input.position, world);
    

	// ���[���h�@��
    input.normal.w = 0;
    output.world_normal = normalize(mul(input.normal, world));

    output.color = material_color;

    output.texcoord = input.texcoord;

    // �ˉeUV���W
    output.projection_texcoord = mul(output.world_position, projection_texture);
    // ���K��
    output.projection_texcoord /= output.projection_texcoord.w;

    // �ˉe����ʒu�𒲐�
    output.projection_texcoord = float4(output.projection_texcoord.x * 0.5f + 0.5f, 0.5f - 0.5f * output.projection_texcoord.y, output.projection_texcoord.z, 1.0f);

	return output;
}