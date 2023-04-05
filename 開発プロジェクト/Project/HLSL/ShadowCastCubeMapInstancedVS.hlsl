#include "ShadowCastCubeMapInstanced.hlsli"

StructuredBuffer<StructuredBufferPrimitive> primitives : register(t0);

#if 0

HSInput main(VSInput input)
{  
    HSInput output = (HSInput) 0;
   

    output.position = input.position;
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    output.instnace_id = input.instnace_id;

    return output;
}

#else 

GSInput main(VSInput input)
{  
    GSInput output = (GSInput) 0;
   
    output.position = mul(float4(input.position.xyz, 1.0f), primitives[input.instnace_id].world);
    output.texcoord = input.texcoord;

    return output;
}

#endif