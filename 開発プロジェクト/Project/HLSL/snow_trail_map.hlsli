struct VSInput
{
    float4 position : POSITION;
};


cbuffer CBuffer : register(b0)
{
    row_major float4x4 wvp;
}