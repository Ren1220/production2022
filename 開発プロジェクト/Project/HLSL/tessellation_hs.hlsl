
#include "tessellation.hlsli"

//https://sites.google.com/site/monshonosuana/directx%E3%81%AE%E8%A9%B1/directx%E3%81%AE%E8%A9%B1-%E7%AC%AC130%E5%9B%9E?authuser=0

ConstantHSOutput CalcHSPatchConstants(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip)
{
	ConstantHSOutput output = (ConstantHSOutput)0;

	[unroll]
    for (uint i = 0; i < NUM_CONTROL_POINTS; ++i)
    {
        output.edge_tess_factor[i] = edge_factor;
    }
    output.inside_tess_factor[0] = output.inside_tess_factor[0] = inside_tess_factor;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HSOutput main(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID)
{
	HSOutput output;

    output.position = ip[i].position;

	return output;
}
