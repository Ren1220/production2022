#include "debug_primitive.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    return material_color;
}