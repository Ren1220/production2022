#include "GausBlur.hlsli"

Texture2D scene_texture : register(t0);

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    output.position = input.position;
    
    // 基準テクセルのUV座標を記録
    float2 texcoord = input.texcoord;
    
    // テクスチャサイズを取得
    float2 texcoord_size;
    float level;
    scene_texture.GetDimensions(0, texcoord_size.x, texcoord_size.y, level);

    // 基準テクセルからＶ座標を+1テクセルからずらすためのオフセットを計算する
    output.texcoord0.xy = float2(0.0f, 1.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+3テクセルからずらすためのオフセットを計算する
    output.texcoord1.xy = float2(0.0f, 3.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+5テクセルからずらすためのオフセットを計算する
    output.texcoord2.xy = float2(0.0f, 5.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+7テクセルからずらすためのオフセットを計算する
    output.texcoord3.xy = float2(0.0f, 7.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+9テクセルからずらすためのオフセットを計算する
    output.texcoord4.xy = float2(0.0f, 9.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+11テクセルからずらすためのオフセットを計算する
    output.texcoord5.xy = float2(0.0f, 11.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+13テクセルからずらすためのオフセットを計算する
    output.texcoord6.xy = float2(0.0f, 13.0f / texcoord_size.y);

    // 基準テクセルからＶ座標を+15テクセルからずらすためのオフセットを計算する
    output.texcoord7.xy = float2(0.0f, 15.0f / texcoord_size.y);

    // オフセットに-1掛けてマイナス方向のオフセットも計算する
    output.texcoord0.zw = output.texcoord0.xy * -1.0f;
    output.texcoord1.zw = output.texcoord1.xy * -1.0f;
    output.texcoord2.zw = output.texcoord2.xy * -1.0f;
    output.texcoord3.zw = output.texcoord3.xy * -1.0f;
    output.texcoord4.zw = output.texcoord4.xy * -1.0f;
    output.texcoord5.zw = output.texcoord5.xy * -1.0f;
    output.texcoord6.zw = output.texcoord6.xy * -1.0f;
    output.texcoord7.zw = output.texcoord7.xy * -1.0f;

    // オフセットに基準テクセルのUV座標を足し算して、
    // 実際にサンプリングするUV座標に変換する
    output.texcoord0 += float4(texcoord, texcoord);
    output.texcoord1 += float4(texcoord, texcoord);
    output.texcoord2 += float4(texcoord, texcoord);
    output.texcoord3 += float4(texcoord, texcoord);
    output.texcoord4 += float4(texcoord, texcoord);
    output.texcoord5 += float4(texcoord, texcoord);
    output.texcoord6 += float4(texcoord, texcoord);
    output.texcoord7 += float4(texcoord, texcoord);


    return output;
}