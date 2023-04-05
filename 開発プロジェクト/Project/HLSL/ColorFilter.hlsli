
cbuffer CBufferColorFilter : register(b3)
{
    float hue_shift;
    float saturation;
    float brightness;
    float contrast;

}

// RGB色空間からHSV色空間に変換する
float3 RGBToHSV(in float3 rgb)
{
    float3 out_hsv = 0;

    float v_max = max(rgb.r, max(rgb.g, rgb.b)); // RGBの中の最大値
    float v_min = min(rgb.r, min(rgb.g, rgb.b)); // 最小値

    float v_delta = v_max - v_min; 
    
    // V（明度）　一番強い色
    out_hsv.z = v_max;
    
    // S（彩度）　最大値と最小値の差を正規化する
    out_hsv.y = (v_delta / v_max) * step(0, v_max); // stepで０除算の結果を０にする
    
    // H（色相）　RGBのうち最大値と最小値の差から求める

    if(out_hsv.y > 0.0f)
    {
        if(rgb.r == v_max)
        {
            out_hsv.x = 60 * ((rgb.g - rgb.b) / v_delta);
        }
        else if(rgb.g == v_max)
        {
            out_hsv.x = 60 * ((rgb.b - rgb.r) / v_delta) + 120;
        }
        else if(rgb.b == v_max)
        {
            out_hsv.x = 60  * ((rgb.r - rgb.g) / v_delta) + 240;
        }

        if (out_hsv.x < 0)
        { 
            out_hsv.x += 360;
        }
    }
    
    #ifdef FORMAT_16
    out_hsv.x = (out_hsv.x / 360 * 65535);
    #endif
    
    return out_hsv;
}

// HSV色空間からRGB色空間に変換する
float3 HSVToRGB(in float3 hsv)
{
    float3 out_rgb = 0;
    
    if (hsv.y == 0)
    {
		// S（彩度）が0と等しいならば無色もしくは灰色
        out_rgb.r = out_rgb.g = out_rgb.b = hsv.z;
    }
    else
    {
		// 色環のH（色相）の位置とS（彩度）、V（明度）からRGB値を算出する
        float v_max = hsv.z;
        float v_min = v_max - (hsv.y * v_max);

        #ifdef FORMAT_16
        hsv.x = hsv.x / 65535.0f * 360.0f;
        #else
        hsv.x %= 360; //	0～360に変換
        #endif
        int hue_i = (int) (hsv.x / 60);
        float hue_f = hsv.x / 60 - hue_i;
        float p = v_max * (1.0f - hsv.y);
        float q = v_max * (1.0f - hsv.y * hue_f);
        float t = v_max * (1.0f - hsv.y * (1 - hue_f));
        if (hue_i == 0)
        {
            out_rgb.r = v_max;
            out_rgb.g = t;
            out_rgb.b = p;
        }
        else if (hue_i == 1)
        {
            out_rgb.r = q;
            out_rgb.g = v_max;
            out_rgb.b = p;
        }
        else if (hue_i == 2)
        {
            out_rgb.r = p;
            out_rgb.g = v_max;
            out_rgb.b = t;
        }
        else if (hue_i == 3)
        {
            out_rgb.r = p;
            out_rgb.g = q;
            out_rgb.b = v_max;
        }
        else if (hue_i == 4)
        {
            out_rgb.r = t;
            out_rgb.g = p;
            out_rgb.b = v_max;
        }
        else if (hue_i == 5)
        {
            out_rgb.r = v_max;
            out_rgb.g = p;
            out_rgb.b = q;
        }
    }

    return out_rgb;
}
