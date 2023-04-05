#include "GeometryBufferInstanced.hlsli"

// 構造化バッファ
StructuredBuffer<StructuredBufferPrimitive> primitives : register(t0);

Texture2DArray displacement_texture : register(t7);

SamplerState point_wrap_sampler_state : register(s0);
SamplerState linear_wrap_sampler_state : register(s1);
SamplerState anisotropic_wrap_sampler_state : register(s2);

[domain("tri")]
PSInput main(
	DSConstantDataInput input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<DSInput, NUM_CONTROL_POINTS> patch)
{
    PSInput output;

    float4 position = float4(
		patch[0].position.xyz * domain.x +
		patch[1].position.xyz * domain.y +
		patch[2].position.xyz * domain.z, 1.0f);
	
    float4 normal = float4(
		patch[0].normal.xyz * domain.x +
		patch[1].normal.xyz * domain.y +
		patch[2].normal.xyz * domain.z, 0.0f);

    float4 tangent = float4(
		patch[0].tangent.xyzw * domain.x +
		patch[1].tangent.xyzw * domain.y +
		patch[2].tangent.xyzw * domain.z);
    float sigma = tangent.w;
    tangent = float4(normalize(tangent.xyz - normal.xyz * dot(normal.xyz, tangent.xyz)), 0.0f);
    float4 binormal = float4(normalize(cross(normal.xyz, tangent.xyz) * sigma), 0.0f);
	
    float3 texcoord = float3(
		patch[0].texcoord * domain.x +
		patch[1].texcoord * domain.y +
		patch[2].texcoord * domain.z, primitives[patch[0].instnace_id].material_index);

	if(primitives[patch[0].instnace_id].is_displacement > 0)
    {
		float displacement = displacement_texture.SampleLevel(linear_wrap_sampler_state, texcoord, 0).r;
		displacement = displacement * 2.0f - 1.0f;
		position.xyz += normal * 0.1f * displacement;
	}

    float3 world_pos = mul(position, primitives[patch[0].instnace_id].world);
	
	output.world_position = float4(world_pos, 1.0f);
    output.position = mul(float4(world_pos, 1.0f), mul(view, projection));
    output.world_normal = mul(normal, primitives[patch[0].instnace_id].world);
    output.world_tangent = mul(tangent, primitives[patch[0].instnace_id].world);
    output.world_binormal= mul(binormal, primitives[patch[0].instnace_id].world);
    output.texcoord = texcoord;
    output.instance_id = patch[0].instnace_id;
	
	return output;
}
