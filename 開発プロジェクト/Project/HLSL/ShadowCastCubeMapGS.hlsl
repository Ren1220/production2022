#include "ShadowCastCubeMap.hlsli"

// DirectXSDK Sample Direct3D10 CubeMapGS.fx

// ジオメトリシェーダーを利用することでキューブマップに対して一度の描画で全ての面に書き込むことが可能
// https://learn.microsoft.com/ja-jp/windows/win32/direct3d11/geometry-shader-stage

[maxvertexcount(18)]
void main(
	triangle GSInput input[3], 
	inout TriangleStream<PSInput> output_stream)
{
    // キューブマップの面のインデックス
    for (int fi = 0; fi < 6; ++fi)
    {
        PSInput output;
        output.rt_index = fi;

        for (int vi = 0; vi < 3; ++vi)
        {
            output.world_position = input[vi].position;
            output.position = mul(output.world_position, mul(cube_map_view[fi], cube_map_proj));
            output.texcoord = input[vi].texcoord;
            output_stream.Append(output);
        }
        
        output_stream.RestartStrip();
    }
}