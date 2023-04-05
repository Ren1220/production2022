#include "geometry_shader.hlsli"

// maxvertexcountはジオメトリシェーダーから出力する最大の頂点数を表す
[maxvertexcount(6)]
void main(
	point GSInput input[1],  // point : プリミティブトポロジーのデータを反映させる
	inout TriangleStream< PSInput > particle_stream // TriangleStream : 三角形ポリゴンデータとして出力する
)
{
    float4x4 wvp = mul(world, mul(view, projection));

    // 移動行列は不要
    //float4x4 vi = mul(view_inverse, transform);
    
    float2 size = input[0].size;
    size.x = size.x * 0.75f;

    PSInput output = (PSInput) 0;
    output.color = input[0].color;
    output.dissolve = input[0].dissolve;
    output.texcoord = float2(0, 0);
    //// 常に画面の方を向く
    //output.position = float4(0, size.y, 0, 0) + mul(input[0].position, wvp);
    //// 普通
    ////output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //particle_stream.Append(output);

    //output.position = float4(0, 0, 0, 0)+ mul(input[0].position, wvp);
    ////output.position = mul(float4(0, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.texcoord = float2(0, 1);
    //particle_stream.Append(output);

    //output.position = float4(size.x, 0, 0, 0)+ mul(input[0].position, wvp);
    ////output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 1);
    //particle_stream.Append(output);

    //particle_stream.RestartStrip();

    ////output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(0, size.y, 0, 0)+ mul(input[0].position, wvp);
    //output.texcoord = float2(0, 0);
    //particle_stream.Append(output);

    ////output.position = mul(float4(size.x, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(size.x, size.y, 0, 0) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 0);
    //particle_stream.Append(output);

    ////output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    //output.position = float4(size.x, 0, 0, 0) + mul(input[0].position, wvp);
    //output.texcoord = float2(1, 1);
    //particle_stream.Append(output);

    //particle_stream.RestartStrip();

        // 常に画面の方を向く

    output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    // 頂点情報を配列に格納する
    particle_stream.Append(output);

    output.position = mul(float4(0, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(0, 1);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 1);
    particle_stream.Append(output);

    // TriangleStreamでは三頂点Append()したら、RestartStrip()を行う
    particle_stream.RestartStrip();

    output.position = mul(float4(0, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(0, 0);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, size.y, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 0);
    particle_stream.Append(output);

    output.position = mul(float4(size.x, 0, 0, 0), wvp) + mul(input[0].position, wvp);
    output.texcoord = float2(1, 1);
    particle_stream.Append(output);

    particle_stream.RestartStrip();


	//// ビュー座標系に変換する
 //   float4 position = mul(input[0].position, view);

	//// 点を面にする
 //   float4 left_top = float4(position.x - input[0].texcoord.x, position.y + input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 right_top = float4(position.x + input[0].texcoord.x, position.y + input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 left_bottom = float4(position.x - input[0].texcoord.x, position.y - input[0].texcoord.y, 0.0f, 0.0f);
 //   float4 right_bottom = float4(position.x + input[0].texcoord.x, position.y - input[0].texcoord.y, 0.0f, 0.0f);

 //   // 左上の頂点の設定
 //   PSInput output = (PSInput)0;
 //   output.color = input[0].color;
 //   output.position = mul(left_top, projection);
 //   output.texcoord = float2(0.0f, 0.0f);
 //   particle_stream.Append(output);

 //   // 右上の頂点の設定
 //   output.position = mul(right_top, projection);
 //   output.texcoord = float2(1.0f, 0.0f);
 //   particle_stream.Append(output);



 //   // 左下の頂点の設定
 //   output.position = mul(left_bottom, projection);
 //   output.texcoord = float2(0.0f, 1.0f);
 //   particle_stream.Append(output);


 //   // 右下の頂点の設定
 //   output.position = mul(right_bottom, projection);
 //   output.texcoord = float2(1.0f, 1.0f);
 //   particle_stream.Append(output);

 //   particle_stream.RestartStrip();
}
