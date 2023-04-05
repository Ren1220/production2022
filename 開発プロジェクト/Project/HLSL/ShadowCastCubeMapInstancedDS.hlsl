#include "ShadowCastCubeMapInstanced.hlsli"

StructuredBuffer<StructuredBufferPrimitive> primitives : register(t0);

Texture2DArray displacement_texture : register(t7);

SamplerState point_wrap_sampler_state : register(s0);
SamplerState linear_wrap_sampler_state : register(s1);
SamplerState anisotropic_wrap_sampler_state : register(s2);

[domain("tri")]
GSInput main(
	DSConstantDataInput input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<DSInput, NUM_CONTROL_POINTS> patch)
{
	GSInput output;

	float4 position = float4(
		patch[0].position.xyz * domain.x +
		patch[1].position.xyz * domain.y + 
		patch[2].position.xyz * domain.z, 1);

    float4 normal = float4(
		patch[0].normal.xyz * domain.x +
		patch[1].normal.xyz * domain.y +
		patch[2].normal.xyz * domain.z, 0.0f);

	output.texcoord = 
		patch[0].position * domain.x +
		patch[1].position * domain.y +
		patch[2].position * domain.z;
	
    if (primitives[patch[0].instnace_id].is_displacement > 0)
    {
        float displacement = displacement_texture.SampleLevel(linear_wrap_sampler_state, float3(output.texcoord, primitives[patch[0].instnace_id].material_index), 0).r;
	//	displacement = displacement * 2.0f - 1.0f;
        position.xyz += normal * 0.1f * displacement;
    }

    output.position = mul(position, primitives[patch[0].instnace_id].world);

	return output;
}
