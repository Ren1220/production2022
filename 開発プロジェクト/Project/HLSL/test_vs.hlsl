#include "test.hlsli"

PSInput main(VSInput input)
{
    // ���[�J���ʒu�����̂܂܃W�I���g���V�F�[�_�[��
    PSInput output;

    output.position = input.position;
    output.texcoord = input.texcoord;
    output.color = input.color;

    return output;
}