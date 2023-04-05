#include "snow_trail_map.hlsli"

float4 main( VSInput input) : SV_POSITION
{
    float4 position = mul(input.position, wvp);
    //float4 position = mul(input.position, mul(world, sky_view_projection));

	return position;


}

