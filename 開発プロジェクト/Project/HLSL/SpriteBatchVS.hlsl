#include "SpriteBatch.hlsli"

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    output.position = mul(float4(input.position.xyz, 1.0f), input.ndc_transform);
    output.texcoord = input.texcoord * input.texcoord_transform.zw + input.texcoord_transform.xy;
    output.color = input.color;

    return output;
}