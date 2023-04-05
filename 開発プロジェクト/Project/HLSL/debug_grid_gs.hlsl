#include "debug_grid.hlsli"


static uint indices[36] =
{
    0, 1, 4, 1, 4, 5,
    4, 5, 6, 5, 6, 7,
    0, 4, 2, 4, 2, 6,
    5, 1, 3, 5, 7, 3,
    6, 7, 2, 7, 2, 3,
    2, 3, 1, 2, 1, 0
};

[maxvertexcount(72)]
void main(
	point GSInput input[1], 
	inout TriangleStream< PSInput > triangle_stream
)
{
    float4x4 wvp = mul(world, mul(view, projection));

    float4 position = input[0].position;

    PSInput output;
    output.color = input[0].color;

    float4 pos = mul(position, wvp);
    float s = 0.001f;
    float c = 0.01f;

    float4 v[8] =
    {
        pos + mul(float4(-s,  c, 100.0f, 0.0f), wvp),
        pos + mul(float4(s,   c, 100.0f, 0.0f), wvp),
        pos + mul(float4(-s, -c, 100.0f, 0.0f), wvp),
        pos + mul(float4(s,  -c, 100.0f, 0.0f), wvp),

        pos + mul(float4(-s,  c, -100.0f, 0.0f), wvp),
        pos + mul(float4(s,   c, -100.0f, 0.0f), wvp),
        pos + mul(float4(-s, -c, -100.0f, 0.0f), wvp),
        pos + mul(float4(s,  -c, -100.0f, 0.0f), wvp)
    };

    for (uint i = 0; i < 36; i += 3)
    {
        output.position = v[indices[i]];
        triangle_stream.Append(output);
        output.position = v[indices[i + 1]];
        triangle_stream.Append(output);
        output.position = v[indices[i + 2]];
        triangle_stream.Append(output);
        triangle_stream.RestartStrip();
    }

    float4 h[8] =
    {
        pos + mul(float4(-100.0f,  c, s, 0.0f), wvp),
        pos + mul(float4(100.0f,   c, s, 0.0f), wvp),
        pos + mul(float4(-100.0f, -c, s, 0.0f), wvp),
        pos + mul(float4(100.0f,  -c, s, 0.0f), wvp),

        pos + mul(float4(-100.0f,  c, -s, 0.0f), wvp),
        pos + mul(float4(100.0f,   c, -s, 0.0f), wvp),
        pos + mul(float4(-100.0f, -c, -s, 0.0f), wvp),
        pos + mul(float4(100.0f,  -c, -s, 0.0f), wvp)
    };

    for (uint i = 0; i < 36; i += 3)
    {
        output.position = h[indices[i]];
        triangle_stream.Append(output);
        output.position = h[indices[i + 1]];
        triangle_stream.Append(output);
        output.position = h[indices[i + 2]];
        triangle_stream.Append(output);
        triangle_stream.RestartStrip();
    }
}