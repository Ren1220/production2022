#include "GeometryBuffer.hlsli"


PSInput main(VSInput input)
{
    float sigma = input.tangent.w;

    PSInput output = (PSInput) 0;
    
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
    
    output.world_position = mul(float4(input.position.xyz, 1.0), world);
    output.position = mul(output.world_position, mul(view, projection));
    output.world_normal = normalize(mul(float4(input.normal.xyz, 0.0), world));

    output.world_tangent = has_tangent ? 
        normalize(mul(float4(input.tangent.xyz, 0.0), world)) :
        float4(1, 0, 0, 1);
    output.world_tangent = float4(normalize(output.world_tangent.xyz - output.world_normal.xyz * dot(output.world_normal.xyz, output.world_tangent.xyz)), 0.0f);
    output.world_tangent.w = has_tangent ? sigma : 1.0f;
    output.world_binormal = float4(normalize(cross(output.world_normal.xyz, output.world_tangent.xyz) * sigma), 0.0f);

    output.texcoord = input.texcoord;

    return output;
}

