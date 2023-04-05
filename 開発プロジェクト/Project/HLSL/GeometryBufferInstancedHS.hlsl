#include "GeometryBufferInstanced.hlsli"

DSConstantDataInput CalcHSPatchConstants(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    DSConstantDataInput output;

    output.edge_tess[0] =
		output.edge_tess[1] =
		output.edge_tess[2] =
		output.inside_tess = 15;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
DSInput main(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    DSInput output;

    output.position = ip[i].position;
    output.normal = ip[i].normal;
    output.tangent = ip[i].tangent;
    output.texcoord = ip[i].texcoord;
    output.instnace_id = ip[i].instnace_id;

    return output;
}
