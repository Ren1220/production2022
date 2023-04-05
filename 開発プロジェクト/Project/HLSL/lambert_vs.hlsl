#include "lambert.hlsli"

PSInput main(VSInput input)
{
    PSInput output;
    
    // WVP���W�ϊ�
    output.position = mul(input.position, mul(world, mul(view, projection)));

	// ���[���h���W�ϊ�
    output.world_position = mul(input.position, world);

    // �@�����̍쐬
    output.normal = normalize(mul(float4(input.normal.xyz, 1), world));
    output.binormal = float3(0.0f, 1.0f, 0.001f); // ���̏�x�N�g��
    output.tangent = normalize(cross(output.binormal, output.normal)); // �O��
    output.binormal = normalize(cross(output.normal, output.tangent)); // �O��

    // ���͐F
    output.color = material_color;
        
    // UV���W
    output.texcoord = float4(input.texcoord, 0, 0);

#if 0
    // �V���h�E�}�b�v�p�̃p�����[�^�v�Z
    {
        // ���C�g�̈ʒu���猩���m�c�b���W���Z�o
        float4 wvp_position = mul(input.position, mul(world, light_view_projection));

        // �m�c�b���W����t�u���W���Z�o
        wvp_position /= wvp_position.w;
        wvp_position.y = -wvp_position.y;
        wvp_position.xy = 0.5f * wvp_position.xy + 0.5f;

        output.shadow_texcoord = wvp_position.xyz;
    }
#endif

    return output;
}