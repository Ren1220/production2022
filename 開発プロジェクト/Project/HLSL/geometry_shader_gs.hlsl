#include "geometry_shader.hlsli"

// maxvertexcount�̓W�I���g���V�F�[�_�[����o�͂���ő�̒��_����\��
[maxvertexcount(6)]
void main(
	point GSInput input[1],  // point : �v���~�e�B�u�g�|���W�[�̃f�[�^�𔽉f������
	inout TriangleStream< PSInput > particle_stream // TriangleStream : �O�p�`�|���S���f�[�^�Ƃ��ďo�͂���
)
{
    float4x4 wvp = mul(world, mul(view, projection));

    // �ړ��s��͕s�v
    //float4x4 vi = mul(view_inverse, transform);
    
    float2 size = input[0].size;
    size.x = size.x * 0.75f;

    PSInput output = (PSInput) 0;
    output.color = input[0].color;
    output.dissolve = input[0].dissolve;
    output.texcoord = float2(0, 0);
    //// ��ɉ�ʂ̕�������
    //output.position = float4(0, size.y, 0, 0) + mul(input[0].position, wvp);
    //// ����
    ////output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //particle_stream.Append(output);

    //output.position = float4(0, 0, 0, 0)+ mul(input[0].position, wvp);
    ////output.position = mul(float4(0, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.texcoord = float2(0, 1);
    //particle_stream.Append(output);

    //output.position = float4(size.x, 0, 0, 0)+ mul(input[0].position, wvp);
    ////output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 1);
    //particle_stream.Append(output);

    //particle_stream.RestartStrip();

    ////output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(0, size.y, 0, 0)+ mul(input[0].position, wvp);
    //output.texcoord = float2(0, 0);
    //particle_stream.Append(output);

    ////output.position = mul(float4(size.x, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(size.x, size.y, 0, 0) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 0);
    //particle_stream.Append(output);

    ////output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(size.x, 0, 0, 0) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 1);
    //particle_stream.Append(output);

    //particle_stream.RestartStrip();

        // ��ɉ�ʂ̕�������

    output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    // ���_����z��Ɋi�[����
    particle_stream.Append(output);

    output.position = mul(float4(0, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(0, 1);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 1);
    particle_stream.Append(output);

    // TriangleStream�ł͎O���_Append()������ARestartStrip()���s��
    particle_stream.RestartStrip();

    output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(0, 0);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 0);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 1);
    particle_stream.Append(output);

    particle_stream.RestartStrip();


	//// �r���[���W�n�ɕϊ�����
 //   float4 position = mul(input[0].position, view);

	//// �_��ʂɂ���
 //   float4 left_top = float4(position.x - input[0].texcoord.x, position.y + input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 right_top = float4(position.x + input[0].texcoord.x, position.y + input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 left_bottom = float4(position.x - input[0].texcoord.x, position.y - input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 right_bottom = float4(position.x + input[0].texcoord.x, position.y - input[0].texcoord.y, 0.0f, 0.0f);

 //   // ����̒��_�̐ݒ�
 //   PSInput output = (PSInput)0;
 //   output.color = input[0].color;
 //   output.position = mul(left_top, projection);
 //   output.texcoord = float2(0.0f, 0.0f);
 //   particle_stream.Append(output);

 //   // �E��̒��_�̐ݒ�
 //   output.position = mul(right_top, projection);
 //   output.texcoord = float2(1.0f, 0.0f);
 //   particle_stream.Append(output);



 //   // �����̒��_�̐ݒ�
 //   output.position = mul(left_bottom, projection);
 //   output.texcoord = float2(0.0f, 1.0f);
 //   particle_stream.Append(output);


 //   // �E���̒��_�̐ݒ�
 //   output.position = mul(right_bottom, projection);
 //   output.texcoord = float2(1.0f, 1.0f);
 //   particle_stream.Append(output);

 //   particle_stream.RestartStrip();
}
