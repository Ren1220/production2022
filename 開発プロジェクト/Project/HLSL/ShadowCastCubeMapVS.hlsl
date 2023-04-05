#include "ShadowCastCubeMap.hlsli"

GSInput main(VSInput input)
{
    float sigma = input.tangent.w;

    GSInput output = (GSInput) 0;
    
    if (skin > -1)
    {
        row_major float4x4 skin_matrix =
        input.weights.x * joint_matrices[input.joints.x] +
        input.weights.y * joint_matrices[input.joints.y] +
        input.weights.z * joint_matrices[input.joints.z] +
        input.weights.w * joint_matrices[input.joints.w];

        input.position = mul(float4(input.position.xyz, 1.0), skin_matrix);
        input.normal = normalize(mul(float4(input.normal.xyz, 0.0), skin_matrix));
        input.tangent = normalize(mul(float4(input.tangent.xyz, 0.0), skin_matrix));
    }

    output.position = mul(input.position, world);
    output.texcoord = input.texcoord;

    return output;
}
