#include "GausBlur.hlsli"

Texture2D scene_texture : register(t0);

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    output.position = input.position;
    
    // ��e�N�Z����UV���W���L�^
    float2 texcoord = input.texcoord;
    
    // �e�N�X�`���T�C�Y���擾
    float2 texcoord_size;
    float level;
    scene_texture.GetDimensions(0, texcoord_size.x, texcoord_size.y, level);

    // ��e�N�Z������u���W��+1�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord0.xy = float2(0.0f, 1.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+3�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord1.xy = float2(0.0f, 3.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+5�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord2.xy = float2(0.0f, 5.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+7�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord3.xy = float2(0.0f, 7.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+9�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord4.xy = float2(0.0f, 9.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+11�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord5.xy = float2(0.0f, 11.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+13�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord6.xy = float2(0.0f, 13.0f / texcoord_size.y);

    // ��e�N�Z������u���W��+15�e�N�Z�����炸�炷���߂̃I�t�Z�b�g���v�Z����
    output.texcoord7.xy = float2(0.0f, 15.0f / texcoord_size.y);

    // �I�t�Z�b�g��-1�|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    output.texcoord0.zw = output.texcoord0.xy * -1.0f;
    output.texcoord1.zw = output.texcoord1.xy * -1.0f;
    output.texcoord2.zw = output.texcoord2.xy * -1.0f;
    output.texcoord3.zw = output.texcoord3.xy * -1.0f;
    output.texcoord4.zw = output.texcoord4.xy * -1.0f;
    output.texcoord5.zw = output.texcoord5.xy * -1.0f;
    output.texcoord6.zw = output.texcoord6.xy * -1.0f;
    output.texcoord7.zw = output.texcoord7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    output.texcoord0 += float4(texcoord, texcoord);
    output.texcoord1 += float4(texcoord, texcoord);
    output.texcoord2 += float4(texcoord, texcoord);
    output.texcoord3 += float4(texcoord, texcoord);
    output.texcoord4 += float4(texcoord, texcoord);
    output.texcoord5 += float4(texcoord, texcoord);
    output.texcoord6 += float4(texcoord, texcoord);
    output.texcoord7 += float4(texcoord, texcoord);


    return output;
}