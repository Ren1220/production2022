#include "GeometryBufferInstanced.hlsli"

// 構造化バッファ
StructuredBuffer<StructuredBufferPrimitive> primitives : register(t0);

#if 0

HSInput main(VSInput input)
{
    float sigma = input.tangent.w;

    HSInput output = (HSInput) 0;

    output.position = input.position;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.texcoord = input.texcoord;
    output.instnace_id = input.instnace_id;
    
    //output.world_position = mul(float4(input.position.xyz, 1.0f), primitives[input.instnace_id].world);
    //output.position = mul(output.world_position, mul(view, projection));
    //output.world_normal = normalize(mul(float4(input.normal.xyz, 0.0), primitives[input.instnace_id].world));
    
    //output.world_tangent = normalize(mul(float4(input.tangent.xyz, 0.0), primitives[input.instnace_id].world));
    //output.world_tangent.w = sigma;
    //output.world_tangent = float4(normalize(output.world_tangent.xyz - output.world_normal.xyz * dot(output.world_normal.xyz, output.world_tangent.xyz)), sigma);
    //output.world_binormal = float4(normalize(cross(output.world_normal.xyz, output.world_tangent.xyz) * sigma), 0.0f);
    
    //output.texcoord = float3(input.texcoord, float(primitives[input.instnace_id].material_index));
    //output.instance_id = input.instnace_id;

    //float3 world_tangent = normalize(mul(float4(1, 0, 0, 0), primitives[input.instance_id].world)); // tangentが適当なので出力時にしっかりとデータを設定する
    
    return output;
}

#else 

PSInput main(VSInput input)
{
    float sigma = input.tangent.w;

    PSInput output = (PSInput) 0;
    
    output.world_position = mul(float4(input.position.xyz, 1.0f), primitives[input.instnace_id].world);
    output.position = mul(output.world_position, mul(view, projection));
    output.world_normal = normalize(mul(float4(input.normal.xyz, 0.0), primitives[input.instnace_id].world));
    
    output.world_tangent = normalize(mul(float4(input.tangent.xyz, 0.0), primitives[input.instnace_id].world));
    output.world_tangent.w = sigma;
    output.world_tangent.xyz = normalize(output.world_tangent.xyz - output.world_normal.xyz * dot(output.world_normal.xyz, output.world_tangent.xyz));
    output.world_binormal = float4(normalize(cross(output.world_normal.xyz, output.world_tangent.xyz) * sigma), 0.0f);
    
    output.texcoord = float3(input.texcoord, float(primitives[input.instnace_id].material_index));
    output.instance_id = input.instnace_id;
    
    return output;
}


#endif
