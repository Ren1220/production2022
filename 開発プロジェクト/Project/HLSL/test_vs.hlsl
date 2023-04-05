#include "test.hlsli"

PSInput main(VSInput input)
{
    // ローカル位置をそのままジオメトリシェーダーへ
    PSInput output;

    output.position = input.position;
    output.texcoord = input.texcoord;
    output.color = input.color;

    return output;
}