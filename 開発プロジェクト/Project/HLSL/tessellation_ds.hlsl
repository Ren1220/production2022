#include "tessellation.hlsli"


[domain("tri")]
DSOutput main(
	ConstantHSOutput input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HSOutput, NUM_CONTROL_POINTS> patch)
{
	DSOutput output;

    output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;

    output.position = mul(output.position, mul(world, mul(view, projection)));

	return output;
}
