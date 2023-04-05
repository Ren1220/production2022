
cbuffer CBufferColorFilter : register(b3)
{
    float hue_shift;
    float saturation;
    float brightness;
    float contrast;

}

// RGB�F��Ԃ���HSV�F��Ԃɕϊ�����
float3 RGBToHSV(in float3 rgb)
{
    float3 out_hsv = 0;

    float v_max = max(rgb.r, max(rgb.g, rgb.b)); // RGB�̒��̍ő�l
    float v_min = min(rgb.r, min(rgb.g, rgb.b)); // �ŏ��l

    float v_delta = v_max - v_min; 
    
    // V�i���x�j�@��ԋ����F
    out_hsv.z = v_max;
    
    // S�i�ʓx�j�@�ő�l�ƍŏ��l�̍��𐳋K������
    out_hsv.y = (v_delta / v_max) * step(0, v_max); // step�łO���Z�̌��ʂ��O�ɂ���
    
    // H�i�F���j�@RGB�̂����ő�l�ƍŏ��l�̍����狁�߂�

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

// HSV�F��Ԃ���RGB�F��Ԃɕϊ�����
float3 HSVToRGB(in float3 hsv)
{
    float3 out_rgb = 0;
    
    if (hsv.y == 0)
    {
		// S�i�ʓx�j��0�Ɠ������Ȃ�Ζ��F�������͊D�F
        out_rgb.r = out_rgb.g = out_rgb.b = hsv.z;
    }
    else
    {
		// �F��H�i�F���j�̈ʒu��S�i�ʓx�j�AV�i���x�j����RGB�l���Z�o����
        float v_max = hsv.z;
        float v_min = v_max - (hsv.y * v_max);

        #ifdef FORMAT_16
        hsv.x = hsv.x / 65535.0f * 360.0f;
        #else
        hsv.x %= 360; //	0�`360�ɕϊ�
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
